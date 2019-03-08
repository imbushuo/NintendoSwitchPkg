#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DevicePath.h>
#include <Library/ArmLib.h>

#include <Protocol/UBootClockManagement.h>
#include <Protocol/Utc/Clock.h>
#include <Protocol/Utc/ErrNo.h>
#include <Protocol/Utc/Mmc.h>
#include <Protocol/Utc/UBootBlk.h>

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Device/Pmc.h>
#include <Library/GpioLib.h>
#include <Protocol/Pmic.h>
#include <Shim/DebugLib.h>
#include <Shim/UBootIo.h>
#include <Shim/TimerLib.h>
#include <Shim/BitOps.h>
#include <Library/Utc/BounceBuf.h>

#include "Include/SdMmc.h"
#include "Include/HostOp.h"
#include "Include/EfiProto.h"

TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL* mClkProtocol;
PMIC_PROTOCOL* mPmicProtocol;
MMC_CONFIG mConfig;
TEGRA_MMC_PRIV mPriv;
struct mmc mMmcInstance;
struct blk_desc mBlkDesc;

void tegra_mmc_set_power(
    struct tegra_mmc_priv *priv,
    unsigned short power)
{
	u8 pwr = 0;
	debug("%s: power = %x\n", __func__, power);

	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = TEGRA_MMC_PWRCTL_SD_BUS_VOLTAGE_V1_8;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = TEGRA_MMC_PWRCTL_SD_BUS_VOLTAGE_V3_0;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = TEGRA_MMC_PWRCTL_SD_BUS_VOLTAGE_V3_3;
			break;
		}
	}
	debug("%s: pwr = %X\n", __func__, pwr);

	/* Set the bus voltage first (if any) */
	writeb(pwr, &priv->reg->pwrcon);
	if (pwr == 0) return;

	/* Now enable bus power */
	pwr |= TEGRA_MMC_PWRCTL_SD_BUS_POWER;
	writeb(pwr, &priv->reg->pwrcon);
}

void tegra_mmc_prepare_data(
    struct tegra_mmc_priv *priv,
    struct mmc_data *data,
    struct bounce_buffer *bbstate
)
{
	unsigned char ctrl;


	debug("buf: %p (%p), data->blocks: %u, data->blocksize: %u\n",
		bbstate->bounce_buffer, bbstate->user_buffer, data->blocks,
		data->blocksize);
	
	ASSERT(((UINTN) bbstate->bounce_buffer) < __UINT32_MAX__);
	writel((u32)(UINTN) bbstate->bounce_buffer, &priv->reg->sysad);
	/*
	 * DMASEL[4:3]
	 * 00 = Selects SDMA
	 * 01 = Reserved
	 * 10 = Selects 32-bit Address ADMA2
	 * 11 = Selects 64-bit Address ADMA2
	 */
	ctrl = readb(&priv->reg->hostctl);
	ctrl &= ~TEGRA_MMC_HOSTCTL_DMASEL_MASK;
	ctrl |= TEGRA_MMC_HOSTCTL_DMASEL_SDMA;
	writeb(ctrl, &priv->reg->hostctl);

	/* We do not handle DMA boundaries, so set it to max (512 KiB) */
	writew((7 << 12) | (data->blocksize & 0xFFF), &priv->reg->blksize);
	writew(data->blocks, &priv->reg->blkcnt);
}

void tegra_mmc_set_transfer_mode(
    struct tegra_mmc_priv *priv,
    struct mmc_data *data
)
{
	unsigned short mode;
	debug(" mmc_set_transfer_mode called\n");
	/*
	 * TRNMOD
	 * MUL1SIN0[5]	: Multi/Single Block Select
	 * RD1WT0[4]	: Data Transfer Direction Select
	 *	1 = read
	 *	0 = write
	 * ENACMD12[2]	: Auto CMD12 Enable
	 * ENBLKCNT[1]	: Block Count Enable
	 * ENDMA[0]	: DMA Enable
	 */
	mode = (TEGRA_MMC_TRNMOD_DMA_ENABLE |
		TEGRA_MMC_TRNMOD_BLOCK_COUNT_ENABLE);

	if (data->blocks > 1)
		mode |= TEGRA_MMC_TRNMOD_MULTI_BLOCK_SELECT;

	if (data->flags & MMC_DATA_READ)
		mode |= TEGRA_MMC_TRNMOD_DATA_XFER_DIR_SEL_READ;

	writew(mode, &priv->reg->trnmod);
}

int tegra_mmc_wait_inhibit(
    struct tegra_mmc_priv *priv,
    struct mmc_cmd *cmd,
    struct mmc_data *data,
    unsigned int timeout
)
{
	/*
	 * PRNSTS
	 * CMDINHDAT[1] : Command Inhibit (DAT)
	 * CMDINHCMD[0] : Command Inhibit (CMD)
	 */
	unsigned int mask = TEGRA_MMC_PRNSTS_CMD_INHIBIT_CMD;

	/*
	 * We shouldn't wait for data inhibit for stop commands, even
	 * though they might use busy signaling
	 */
	if ((data == NULL) && (cmd->resp_type & MMC_RSP_BUSY))
		mask |= TEGRA_MMC_PRNSTS_CMD_INHIBIT_DAT;

	while (readl(&priv->reg->prnsts) & mask) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return -1;
		}
		timeout--;
		udelay(1000);
	}

	return 0;
}

int tegra_mmc_send_cmd_bounced(
    struct tegra_mmc_priv *priv, 
    struct mmc_cmd *cmd,
    struct mmc_data *data,
    struct bounce_buffer *bbstate
)
{
	int flags, i;
	int result;
	unsigned int mask = 0;
	unsigned int retry = 0x100000;
	debug(" mmc_send_cmd called\n");

	result = tegra_mmc_wait_inhibit(priv, cmd, data, 10 /* ms */);

	if (result < 0)
		return result;

	if (data)
		tegra_mmc_prepare_data(priv, data, bbstate);

	debug("cmd->arg: %08x\n", cmd->cmdarg);
	writel(cmd->cmdarg, &priv->reg->argument);

	if (data)
		tegra_mmc_set_transfer_mode(priv, data);

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
	{
		debug("MMC_RSP_136 or MMC_RSP_BUSY cannot coexist \n");
		return -1;
	}

	/*
	 * CMDREG
	 * CMDIDX[13:8]	: Command index
	 * DATAPRNT[5]	: Data Present Select
	 * ENCMDIDX[4]	: Command Index Check Enable
	 * ENCMDCRC[3]	: Command CRC Check Enable
	 * RSPTYP[1:0]
	 *	00 = No Response
	 *	01 = Length 136
	 *	10 = Length 48
	 *	11 = Length 48 Check busy after response
	 */
	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = TEGRA_MMC_CMDREG_RESP_TYPE_SELECT_NO_RESPONSE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = TEGRA_MMC_CMDREG_RESP_TYPE_SELECT_LENGTH_136;
	else if (cmd->resp_type & MMC_RSP_BUSY)
		flags = TEGRA_MMC_CMDREG_RESP_TYPE_SELECT_LENGTH_48_BUSY;
	else
		flags = TEGRA_MMC_CMDREG_RESP_TYPE_SELECT_LENGTH_48;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= TEGRA_MMC_TRNMOD_CMD_CRC_CHECK;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= TEGRA_MMC_TRNMOD_CMD_INDEX_CHECK;
	if (data)
		flags |= TEGRA_MMC_TRNMOD_DATA_PRESENT_SELECT_DATA_TRANSFER;

	debug("cmd: %d\n", cmd->cmdidx);

	writew((cmd->cmdidx << 8) | flags, &priv->reg->cmdreg);

	for (i = 0; i < retry; i++) 
	{
		mask = readl(&priv->reg->norintsts);
		/* Command Complete */
		if (mask & TEGRA_MMC_NORINTSTS_CMD_COMPLETE) 
		{
			if (!data) writel(mask, &priv->reg->norintsts);
			break;
		}
	}

	if (i == retry) 
	{
		printf("%s: waiting for status update\n", __func__);
		writel(mask, &priv->reg->norintsts);
		return -ETIMEDOUT;
	}

	if (mask & TEGRA_MMC_NORINTSTS_CMD_TIMEOUT) 
	{
		/* Timeout Error */
		debug("timeout: %08x cmd %d \n", mask, cmd->cmdidx);
		writel(mask, &priv->reg->norintsts);
		return -ETIMEDOUT;
	} 
	else if (mask & TEGRA_MMC_NORINTSTS_ERR_INTERRUPT) 
	{
		/* Error Interrupt */
		debug("error: %08x cmd %d \n", mask, cmd->cmdidx);
		writel(mask, &priv->reg->norintsts);
		return -1;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) 
	{
		if (cmd->resp_type & MMC_RSP_136) 
		{
			/* CRC is stripped so we need to do some shifting. */
			for (i = 0; i < 4; i++) 
			{
				unsigned long offset = (unsigned long)
					(&priv->reg->rspreg3 - i);
				cmd->response[i] = readl(offset) << 8;

				if (i != 3) 
				{
					cmd->response[i] |= readb(offset - 1);
				}
				debug("cmd->resp[%d]: %08x\n", i, cmd->response[i]);
			}
		} 
		else if (cmd->resp_type & MMC_RSP_BUSY) 
		{
			for (i = 0; i < retry; i++) 
			{
				/* PRNTDATA[23:20] : DAT[3:0] Line Signal */
				if (readl(&priv->reg->prnsts)
					& (1 << 20))	/* DAT[0] */
					break;
			}

			if (i == retry) 
			{
				printf("%s: card is still busy\n", __func__);
				writel(mask, &priv->reg->norintsts);
				return -ETIMEDOUT;
			}

			cmd->response[0] = readl(&priv->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		} 
		else 
		{
			cmd->response[0] = readl(&priv->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		}
	}

	if (data) 
	{
		unsigned long start = get_timer(0);

		while (1) 
		{
			mask = readl(&priv->reg->norintsts);

			if (mask & TEGRA_MMC_NORINTSTS_ERR_INTERRUPT) 
			{
				/* Error Interrupt */
				writel(mask, &priv->reg->norintsts);
				printf("%s: error during transfer: 0x%08x\n",
						__func__, mask);
				return -1;
			} 
			else if (mask & TEGRA_MMC_NORINTSTS_DMA_INTERRUPT) 
			{
				/*
				 * DMA Interrupt, restart the transfer where
				 * it was interrupted.
				 */
				unsigned int address = readl(&priv->reg->sysad);

				debug("DMA end\n");
				writel(TEGRA_MMC_NORINTSTS_DMA_INTERRUPT,
				       &priv->reg->norintsts);
				writel(address, &priv->reg->sysad);
			} 
			else if (mask & TEGRA_MMC_NORINTSTS_XFER_COMPLETE) 
			{
				/* Transfer Complete */
				debug("r/w is done\n");
				break;
			} 
			else if (get_timer(start) > 8000UL) 
			{
				writel(mask, &priv->reg->norintsts);
				printf("%s: MMC Timeout\n"
				       "    Interrupt status        0x%08x\n"
				       "    Interrupt status enable 0x%08x\n"
				       "    Interrupt signal enable 0x%08x\n"
				       "    Present status          0x%08x\n",
				       __func__, mask,
				       readl(&priv->reg->norintstsen),
				       readl(&priv->reg->norintsigen),
				       readl(&priv->reg->prnsts));
				return -1;
			}
		}
		writel(mask, &priv->reg->norintsts);
	}

	udelay(1000);
	return 0;
}

int tegra_mmc_send_cmd(
    struct tegra_mmc_priv *priv, 
    struct mmc_cmd *cmd,
    struct mmc_data *data
)
{
	void *buf;
	unsigned int bbflags;
	size_t len;
	struct bounce_buffer bbstate;
	int ret;

	if (data) 
	{
		if (data->flags & MMC_DATA_READ) 
		{
			buf = data->dest;
			bbflags = GEN_BB_WRITE;
		} 
		else 
		{
			buf = (void *)data->src;
			bbflags = GEN_BB_READ;
		}
		len = data->blocks * data->blocksize;

		bounce_buffer_start(&bbstate, buf, len, bbflags);
	}

	ret = tegra_mmc_send_cmd_bounced(priv, cmd, data, &bbstate);

	if (data)
	{
		bounce_buffer_stop(&bbstate);
	}

	return ret;
}

void tegra_mmc_pad_init(struct tegra_mmc_priv *priv)
{
    // Nothing to do for Tegra 210
    return;
}

void tegra_mmc_change_clock(struct tegra_mmc_priv *priv, uint clock)
{
	ulong rate;
	int div;
	unsigned short clk;
	unsigned long timeout;

	debug(" mmc_change_clock called\n");

	/*
	 * Change Tegra SDMMCx clock divisor here. Source is PLLP_OUT0
	 */
	if (clock == 0) goto out;
    
	rate = mClkProtocol->SetRate(PERIPH_ID_SDMMC1, clock);
	div = (rate + clock - 1) / clock;
	debug("div = %d\n", div);

	writew(0, &priv->reg->clkcon);

	/*
	 * CLKCON
	 * SELFREQ[15:8]	: base clock divided by value
	 * ENSDCLK[2]		: SD Clock Enable
	 * STBLINTCLK[1]	: Internal Clock Stable
	 * ENINTCLK[0]		: Internal Clock Enable
	 */
	div >>= 1;
	clk = ((div << TEGRA_MMC_CLKCON_SDCLK_FREQ_SEL_SHIFT) |
	       TEGRA_MMC_CLKCON_INTERNAL_CLOCK_ENABLE);
	writew(clk, &priv->reg->clkcon);

	/* Wait max 10 ms */
	timeout = 10;
	while (!(readw(&priv->reg->clkcon) &
		 TEGRA_MMC_CLKCON_INTERNAL_CLOCK_STABLE)) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return;
		}
		timeout--;
		udelay(1000);
	}

	clk |= TEGRA_MMC_CLKCON_SD_CLOCK_ENABLE;
	writew(clk, &priv->reg->clkcon);

	debug("mmc_change_clock: clkcon = %08X\n", clk);

out:
	priv->clock = clock;
}

int tegra_mmc_set_ios(
    struct mmc* mMmcInstance
)
{
	struct tegra_mmc_priv *priv = &mPriv;
	unsigned char ctrl;
	debug(" mmc_set_ios called\n");

	debug("bus_width: %x, clock: %d\n", mMmcInstance->bus_width, mMmcInstance->clock);

	/* Change clock first */
	tegra_mmc_change_clock(priv, mMmcInstance->clock);

	ctrl = readb(&priv->reg->hostctl);

	/*
	 * WIDE8[5]
	 * 0 = Depend on WIDE4
	 * 1 = 8-bit mode
	 * WIDE4[1]
	 * 1 = 4-bit mode
	 * 0 = 1-bit mode
	 */
	if (mMmcInstance->bus_width == 8)
		ctrl |= (1 << 5);
	else if (mMmcInstance->bus_width == 4)
		ctrl |= (1 << 1);
	else
		ctrl &= ~(1 << 1 | 1 << 5);

	writeb(ctrl, &priv->reg->hostctl);
	debug("mmc_set_ios: hostctl = %08X\n", ctrl);

	return 0;
}

EFI_STATUS
TegraMmcReset
(
    PTEGRA_MMC_PRIV priv
)
{
    unsigned int timeout;
	debug(" mmc_reset called\n");

    /*
	 * RSTALL[0] : Software reset for all
	 * 1 = reset
	 * 0 = work
	 */
	writeb(TEGRA_MMC_SWRST_SW_RESET_FOR_ALL, &priv->reg->swrst);

	priv->clock = 0;

	/* Wait max 100 ms */
	timeout = 100;

    /* hw clears the bit when it's done */
	while (readb(&priv->reg->swrst) & TEGRA_MMC_SWRST_SW_RESET_FOR_ALL) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return EFI_TIMEOUT;
		}
		timeout--;
		udelay(1000);
	}

    /* Set SD bus voltage & enable bus power */
	tegra_mmc_set_power(priv, fls(mMmcInstance.cfg->voltages) - 1);
	debug("%s: power control = %02X, host control = %02X\n", __func__,
		readb(&priv->reg->pwrcon), readb(&priv->reg->hostctl));

	/* Make sure SDIO pads are set up */
	tegra_mmc_pad_init(priv);

    return EFI_SUCCESS;
}

EFI_STATUS
TegraMmcInit
(
    VOID
)
{
    PTEGRA_MMC_PRIV priv = &mPriv;
    unsigned int mask;
    EFI_STATUS Status;
	debug(" tegra_mmc_init called\n");

    Status = TegraMmcReset(priv);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "SDMMC reset failed \n"));
        goto exit;
    }

	priv->version = readw(&priv->reg->hcver);
	debug("host version = %x\n", priv->version);

    /* mask all */
	writel(0xffffffff, &priv->reg->norintstsen);
	writel(0xffffffff, &priv->reg->norintsigen);

    writeb(0xe, &priv->reg->timeoutcon);	/* TMCLK * 2^27 */
	/*
	 * NORMAL Interrupt Status Enable Register init
	 * [5] ENSTABUFRDRDY : Buffer Read Ready Status Enable
	 * [4] ENSTABUFWTRDY : Buffer write Ready Status Enable
	 * [3] ENSTADMAINT   : DMA boundary interrupt
	 * [1] ENSTASTANSCMPLT : Transfre Complete Status Enable
	 * [0] ENSTACMDCMPLT : Command Complete Status Enable
	*/
	mask = readl(&priv->reg->norintstsen);
	mask &= ~(0xffff);
	mask |= (TEGRA_MMC_NORINTSTSEN_CMD_COMPLETE |
		 TEGRA_MMC_NORINTSTSEN_XFER_COMPLETE |
		 TEGRA_MMC_NORINTSTSEN_DMA_INTERRUPT |
		 TEGRA_MMC_NORINTSTSEN_BUFFER_WRITE_READY |
		 TEGRA_MMC_NORINTSTSEN_BUFFER_READ_READY);
	writel(mask, &priv->reg->norintstsen);

	/*
	 * NORMAL Interrupt Signal Enable Register init
	 * [1] ENSTACMDCMPLT : Transfer Complete Signal Enable
	 */
	mask = readl(&priv->reg->norintsigen);
	mask &= ~(0xffff);
	mask |= TEGRA_MMC_NORINTSIGEN_XFER_COMPLETE;
	writel(mask, &priv->reg->norintsigen);

exit:
    return Status;
}

EFI_STATUS
SdControllerProbe
(
    VOID
)
{
    int ret = 0;

	// All init
	ZeroMem(&mBlkDesc, sizeof(struct blk_desc));
	ZeroMem(&mMmcInstance, sizeof(struct mmc));
	ZeroMem(&mPriv, sizeof(TEGRA_MMC_PRIV));
	ZeroMem(&mConfig, sizeof(MMC_CONFIG));

    // Set configuration
    mConfig.voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
    mConfig.host_caps = 0;
	
    // bus-width = <4>;
    mConfig.host_caps |= MMC_MODE_4BIT;
    mConfig.host_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;

    /*
	 * min freq is for card identification, and is the highest
	 *  low-speed SDIO card frequency (actually 400KHz)
	 * max freq is highest HS eMMC clock as per the SD/MMC spec
	 *  (actually 52MHz)
	 */
	mConfig.f_min = 375000;
	mConfig.f_max = 48000000;

	mConfig.b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	// Bound to instance
	mMmcInstance.cfg = &mConfig;
	mMmcInstance.clock = mConfig.f_min;

    // sdhci@700b0000
    mPriv.reg = (VOID*) (UINTN) 0x700b0000;

    // Reset controller 1, &tegra_car 14
    mClkProtocol->AssertRst(PERIPH_ID_SDMMC1);

    // Clock enable, TEGRA210_CLK_SDMMC1
    mClkProtocol->EnableClk(PERIPH_ID_SDMMC1);

    // Set Rate
    ret = mClkProtocol->SetRate(PERIPH_ID_SDMMC1, 20000000);
    if (IS_ERR_VALUE(ret))
    {
        DEBUG((EFI_D_ERROR, "SD card set rate failed \n"));
        return EFI_DEVICE_ERROR;
    }

    // De-assert
    mClkProtocol->DeassertRst(PERIPH_ID_SDMMC1);

    // Detect card
    if(!!gpio_read(GPIO_PORT_Z, GPIO_PIN_1))
    {
        DEBUG((EFI_D_ERROR, "SD card not found \n"));
        return EFI_NOT_FOUND;
    }

    // power-gpios = <&gpio TEGRA_GPIO(E, 4) GPIO_ACTIVE_HIGH>;
	gpio_config(GPIO_PORT_E, GPIO_PIN_4, GPIO_MODE_GPIO);
	gpio_write(GPIO_PORT_E, GPIO_PIN_4, GPIO_HIGH);
	gpio_output_enable(GPIO_PORT_E, GPIO_PIN_4, GPIO_OUTPUT_ENABLE);

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SdMmcDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
	BIO_INSTANCE *Instance;

    Status = gBS->LocateProtocol(
        &gTegraUBootClockManagementProtocolGuid,
        NULL,
        (VOID**) &mClkProtocol
    );

    if (EFI_ERROR(Status)) goto exit;

    Status = gBS->LocateProtocol(
        &gPmicProtocolGuid,
        NULL,
        (VOID**) &mPmicProtocol
    );

    if (EFI_ERROR(Status)) goto exit;

    Status = SdControllerProbe();
    if (EFI_ERROR(Status)) goto exit;

	Status = TegraMmcInit();
	if (EFI_ERROR(Status)) goto exit;

	// Check some commands
	int ret = SdFxInit();
	if (ret) 
	{
		Status = EFI_DEVICE_ERROR;
		goto exit;
	}
	ret = SdFxInitFinalize();
	if (ret) 
	{
		Status = EFI_DEVICE_ERROR;
		goto exit;
	}

	if (mMmcInstance.has_init == 1)
	{
		// Run a self test
		//
		// Dump content to verify Read status
		//
		UINT8 BlkDump[512];
		ZeroMem(BlkDump, 512);
		BOOLEAN FoundMbr = FALSE;
		for (UINTN i = 0; i <= MIN(mBlkDesc.lba, 50); i++)
		{
			int blk = mmc_bread(i, 1, &BlkDump);
			if (blk)
			{
				if (BlkDump[510] == 0x55 && BlkDump[511] == 0xAA)
				{
					DEBUG((EFI_D_INFO, "MBR found at %d \n", i));
					FoundMbr = TRUE;
					break;
				}
				DEBUG((EFI_D_INFO, "MBR not found at %d \n", i));
			}
		}

		if (!FoundMbr)
		{
			DEBUG((EFI_D_ERROR, "(Protective) MBR not found \n"));
			CpuDeadLoop();
		}
		
		// Install EFI protocol
		ASSERT(mBlkDesc.lba != 0);
		ASSERT(mBlkDesc.blksz != 0);
		Status = BioInstanceContructor(&Instance);
		if (EFI_ERROR(Status)) goto exit;

		Instance->BlockMedia.BlockSize = mBlkDesc.blksz;
		Instance->BlockMedia.LastBlock = mBlkDesc.lba;
		Status = gBS->InstallMultipleProtocolInterfaces(
			&Instance->Handle,
			&gEfiBlockIoProtocolGuid,    
			&Instance->BlockIo,
			&gEfiDevicePathProtocolGuid, 
			&Instance->DevicePath,
			NULL
		);
	}

exit:
    return Status;
}