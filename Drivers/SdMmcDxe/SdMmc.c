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
#include <Device/StaticDevices.h>

#include <Protocol/UBootClockManagement.h>
#include <Protocol/Utc/Clock.h>
#include <Protocol/Utc/ErrNo.h>

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Device/Pmc.h>
#include <Library/PinmuxLib.h>
#include <Library/GpioLib.h>
#include <Protocol/Pmic.h>
#include <Shim/DebugLib.h>
#include <Shim/UBootIo.h>
#include <Shim/TimerLib.h>
#include <Shim/BitOps.h>

#include "Include/SdMmc.h"

TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL* mClkProtocol;
PMIC_PROTOCOL* mPmicProtocol;
MMC_CONFIG mConfig;
TEGRA_MMC_PRIV mPriv;

static void tegra_mmc_set_power(
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

static void tegra_mmc_pad_init(struct tegra_mmc_priv *priv)
{
    // Nothing to do for Tegra 210
    return;
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
	tegra_mmc_set_power(priv, fls(mConfig.voltages) - 1);
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
    
    // Power on controller
    APB_MISC(APB_MISC_GP_SDMMC1_CLK_LPBK_CONTROL) = 1;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_CLK)  = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_CMD)  = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED | PINMUX_PULL_UP;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_DAT3) = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED | PINMUX_PULL_UP;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_DAT2) = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED | PINMUX_PULL_UP;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_DAT1) = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED | PINMUX_PULL_UP;
	PINMUX_AUX(PINMUX_AUX_SDMMC1_DAT0) = PINMUX_DRIVE_2X | PINMUX_INPUT_ENABLE | PINMUX_PARKED | PINMUX_PULL_UP;

    // Make sure the SDMMC1 controller is powered.
	PMC(APBDEV_PMC_NO_IOPOWER) &= ~(1 << 12);

	// Assume 3.3V SD card voltage.
	PMC(APBDEV_PMC_PWR_DET_VAL) |= (1 << 12);

    // Enable SD card power.
    mPmicProtocol->SetRegulatorVoltage(REGULATOR_LDO2, 3300000);
    mPmicProtocol->EnableRegulator(REGULATOR_LDO2);
    gBS->Stall(1000);

    // For good measure.
	APB_MISC(APB_MISC_GP_SDMMC1_PAD_CFGPADCTRL) = 0x10000000;

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

    // Also get GPIOs, initialize MUX
    // cd-gpios = <&gpio TEGRA_GPIO(Z, 1) GPIO_ACTIVE_LOW>;
    // GPIO control, pull up.
    PINMUX_AUX(PINMUX_AUX_GPIO_PZ1) = PINMUX_INPUT_ENABLE | PINMUX_PULL_UP | 1;
	APB_MISC(APB_MISC_GP_VGPIO_GPIO_MUX_SEL) = 0;
	gpio_config(GPIO_PORT_Z, GPIO_PIN_1, GPIO_MODE_GPIO);
	gpio_output_enable(GPIO_PORT_Z, GPIO_PIN_1, GPIO_OUTPUT_DISABLE);
    gBS->Stall(100);

    // Detect card
    if(!!gpio_read(GPIO_PORT_Z, GPIO_PIN_1))
    {
        DEBUG((EFI_D_ERROR, "SD card not found \n"));
        return EFI_NOT_FOUND;
    }

    // power-gpios = <&gpio TEGRA_GPIO(E, 4) GPIO_ACTIVE_HIGH>;
    // GPIO control, pull down.
    PINMUX_AUX(PINMUX_AUX_DMIC3_CLK) = PINMUX_INPUT_ENABLE | PINMUX_PULL_DOWN | 1;
	gpio_config(GPIO_PORT_E, GPIO_PIN_4, GPIO_MODE_GPIO);
	gpio_write(GPIO_PORT_E, GPIO_PIN_4, GPIO_HIGH);
	gpio_output_enable(GPIO_PORT_E, GPIO_PIN_4, GPIO_OUTPUT_ENABLE);
    gBS->Stall(1000);

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

exit:
    ASSERT_EFI_ERROR(Status);
    return Status;
}