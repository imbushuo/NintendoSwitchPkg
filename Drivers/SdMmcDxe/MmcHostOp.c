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
#include <Shim/MemAlign.h>
#include <Library/Utc/BounceBuf.h>

#include "Include/TegraMmc.h"

extern struct mmc mMmcInstance;
extern TEGRA_MMC_PRIV mPriv;
extern struct blk_desc mBlkDesc;

static const unsigned int sd_au_size[] = {
	0, SIZE_16KB / 512,	SIZE_32KB / 512,
	SIZE_64KB / 512, SIZE_128KB / 512, SIZE_256KB / 512,
	SIZE_512KB / 512, SIZE_1MB / 512, SIZE_2MB / 512,
	SIZE_4MB / 512,	SIZE_8MB / 512, (SIZE_8MB + SIZE_4MB) / 512,
	SIZE_16MB / 512, (SIZE_16MB + SIZE_8MB) / 512, SIZE_32MB / 512, SIZE_64MB / 512,
};

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
	10000,
	100000,
	1000000,
	10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const u8 multipliers[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

void mmc_set_clock(struct mmc *mmc, uint clock)
{
	if (clock > mmc->cfg->f_max)
		clock = mmc->cfg->f_max;

	if (clock < mmc->cfg->f_min)
		clock = mmc->cfg->f_min;

	mmc->clock = clock;
	tegra_mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, uint width)
{
	mmc->bus_width = width;
	tegra_mmc_set_ios(mmc);
}

static int mmc_go_idle()
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

	if (err) return err;

	udelay(2000);

	return 0;
}

static int mmc_send_if_cond()
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;

	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mMmcInstance.cfg->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

	if (err) return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return -EOPNOTSUPP;
	else
		mMmcInstance.version = SD_VERSION_2;

	return 0;
}

static int sd_send_op_cond()
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;

	while (1) 
    {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;

		err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

		if (err) return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */
		cmd.cmdarg = mMmcInstance.cfg->voltages & 0xff8000;

		if (mMmcInstance.version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

		if (err) return err;
		if (cmd.response[0] & OCR_BUSY) break;
		if (timeout-- <= 0) return -EOPNOTSUPP;

		udelay(1000);
	}

	if (mMmcInstance.version != SD_VERSION_2)
		mMmcInstance.version = SD_VERSION_1_0;

	mMmcInstance.ocr = cmd.response[0];
	mMmcInstance.high_capacity = ((mMmcInstance.ocr & OCR_HCS) == OCR_HCS);
	mMmcInstance.rca = 0;

	return 0;
}

static int mmc_send_op_cond_iter(struct mmc *mmc, int use_arg)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = MMC_CMD_SEND_OP_COND;
	cmd.resp_type = MMC_RSP_R3;
	cmd.cmdarg = 0;
	if (use_arg)
		cmd.cmdarg = OCR_HCS |
			(mmc->cfg->voltages &
			(mmc->ocr & OCR_VOLTAGE_MASK)) |
			(mmc->ocr & OCR_ACCESS_MODE);

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
	if (err) return err;
	mmc->ocr = cmd.response[0];
	return 0;
}

static int mmc_send_op_cond(struct mmc *mmc)
{
	int err, i;

	/* Some cards seem to need this */
	mmc_go_idle();

 	/* Asking to the card its capabilities */
	for (i = 0; i < 2; i++) {
		err = mmc_send_op_cond_iter(mmc, i != 0);
		if (err)
			return err;

		/* exit if not busy (flag seems to be inverted) */
		if (mmc->ocr & OCR_BUSY)
			break;
	}
	mmc->op_cond_pending = 1;
	return 0;
}

static int mmc_complete_op_cond(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	uint start;
	int err;

	mmc->op_cond_pending = 0;
	if (!(mmc->ocr & OCR_BUSY)) {
		/* Some cards seem to need this */
		mmc_go_idle();

		start = get_timer(0);
		while (1) {
			err = mmc_send_op_cond_iter(mmc, 1);
			if (err)
				return err;
			if (mmc->ocr & OCR_BUSY)
				break;
			if (get_timer(start) > timeout)
				return -EOPNOTSUPP;
			udelay(100);
		}
	}

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 1;

	return 0;
}

static int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return tegra_mmc_send_cmd(&mPriv, &cmd, &data);
}

int mmc_send_status(struct mmc *mmc, int timeout)
{
	struct mmc_cmd cmd;
	int err, retries = 5;

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;

	while (1) 
	{
		err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
		if (!err) 
		{
			if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
			    (cmd.response[0] & MMC_STATUS_CURR_STATE) !=
			     MMC_STATE_PRG)
				break;
			else if (cmd.response[0] & MMC_STATUS_MASK) {
				printf("Status Error: 0x%08X\n", cmd.response[0]);
				return -ECOMM;
			}
		} 
		else if (--retries < 0)
		{
			return err;
		}

		if (timeout-- <= 0) break;
		udelay(1000);
	}

	if (timeout <= 0) 
	{
		printf("Timeout waiting card ready\n");
		return -ETIMEDOUT;
	}

	return 0;
}

int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	int retries = 3;
	int ret;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
				 (index << 16) |
				 (value << 8);

	while (retries > 0) 
	{
		ret = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
		/* Waiting for the ready status */
		if (!ret) 
		{
			ret = mmc_send_status(mmc, timeout);
			return ret;
		}

		retries--;
	}

	return ret;
}

static int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	ALLOC_CACHE_ALIGN_BUFFER(uint, scr, 2);
	ALLOC_CACHE_ALIGN_BUFFER(uint, switch_status, 16);
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

	if (err) return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	timeout = 3;

retry_scr:
	data.dest = (char *)scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, &data);

	if (err) 
	{
		if (timeout--) goto retry_scr;
		return err;
	}

	mmc->scr[0] = be32_to_cpu(scr[0]);
	mmc->scr[1] = be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) 
	{
	case 0:
		mmc->version = SD_VERSION_1_0;
		break;
	case 1:
		mmc->version = SD_VERSION_1_10;
		break;
	case 2:
		mmc->version = SD_VERSION_2;
		if ((mmc->scr[0] >> 15) & 0x1)
			mmc->version = SD_VERSION_3;
		break;
	default:
		mmc->version = SD_VERSION_1_0;
		break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0) return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *)switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY)) break;
	}

	/* If high-speed isn't supported, we return */
	if (!(be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED)) return 0;

	/*
	 * If the host doesn't support SD_HIGHSPEED, do not switch card to
	 * HIGHSPEED mode even if the card support SD_HIGHSPPED.
	 * This can avoid furthur problem when the card runs in different
	 * mode between the host.
	 */
	if (!((mmc->cfg->host_caps & MMC_MODE_HS_52MHz) &&
		(mmc->cfg->host_caps & MMC_MODE_HS)))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)switch_status);

	if (err)
		return err;

	if ((be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;

	return 0;
}

static int sd_read_ssr(struct mmc *mmc)
{
	int err, i;
	struct mmc_cmd cmd;
	ALLOC_CACHE_ALIGN_BUFFER(uint, ssr, 16);
	struct mmc_data data;
	int timeout = 3;
	unsigned int au, eo, et, es;

	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
	if (err) return err;

	cmd.cmdidx = SD_CMD_APP_SD_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

retry_ssr:
	data.dest = (char *)ssr;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, &data);
	if (err) 
	{
		if (timeout--) goto retry_ssr;
		return err;
	}

	for (i = 0; i < 16; i++)
		ssr[i] = be32_to_cpu(ssr[i]);

	au = (ssr[2] >> 12) & 0xF;
	if ((au <= 9) || (mmc->version == SD_VERSION_3)) {
		mmc->ssr.au = sd_au_size[au];
		es = (ssr[3] >> 24) & 0xFF;
		es |= (ssr[2] & 0xFF) << 8;
		et = (ssr[3] >> 18) & 0x3F;
		if (es && et) {
			eo = (ssr[3] >> 16) & 0x3;
			mmc->ssr.erase_timeout = (et * 1000) / es;
			mmc->ssr.erase_offset = eo * 1000;
		}
	} else {
		debug("Invalid Allocation Unit Size.\n");
	}

	return 0;
}

static int mmc_set_capacity(struct mmc *mmc, int part_num)
{
	switch (part_num) 
	{
	case 0:
		mmc->capacity = mmc->capacity_user;
		break;
	case 1:
	case 2:
		mmc->capacity = mmc->capacity_boot;
		break;
	case 3:
		mmc->capacity = mmc->capacity_rpmb;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		mmc->capacity = mmc->capacity_gp[part_num - 4];
		break;
	default:
		return -1;
	}

	mBlkDesc.lba = lldiv(mmc->capacity, mmc->read_bl_len);
	return 0;
}

static int mmc_startup(struct mmc *mmc)
{
	int err, i;
	uint mult, freq;
	u64 cmult, csize, capacity;
	struct mmc_cmd cmd;
	ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, test_csd, MMC_MAX_BLOCK_LEN);
	bool has_parts = false;
	bool part_completed;

	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
	if (err) goto exit;
	CopyMem(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = mmc->rca << 16;
	cmd.resp_type = MMC_RSP_R6;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
	if (err) goto exit;

	if (IS_SD(mmc)) mmc->rca = (cmd.response[0] >> 16) & 0xffff;

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;

	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
	if (err) goto exit;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) 
	{
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) 
		{
		case 0:
			mmc->version = MMC_VERSION_1_2;
			break;
		case 1:
			mmc->version = MMC_VERSION_1_4;
			break;
		case 2:
			mmc->version = MMC_VERSION_2_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_3;
			break;
		case 4:
			mmc->version = MMC_VERSION_4;
			break;
		default:
			mmc->version = MMC_VERSION_1_2;
			break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;
	mmc->dsr_imp = ((cmd.response[1] >> 12) & 0x1);
	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	if (mmc->high_capacity) 
	{
		csize = (mmc->csd[1] & 0x3f) << 16
			| (mmc->csd[2] & 0xffff0000) >> 16;
		cmult = 8;
	} 
	else 
	{
		csize = (mmc->csd[1] & 0x3ff) << 2
			| (mmc->csd[2] & 0xc0000000) >> 30;
		cmult = (mmc->csd[2] & 0x00038000) >> 15;
	}

	mmc->capacity_user = (csize + 1) << (cmult + 2);
	mmc->capacity_user *= mmc->read_bl_len;
	mmc->capacity_boot = 0;
	mmc->capacity_rpmb = 0;
	for (i = 0; i < 4; i++)
		mmc->capacity_gp[i] = 0;

	if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

	if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

	if ((mmc->dsr_imp) && (0xffffffff != mmc->dsr)) {
		cmd.cmdidx = MMC_CMD_SET_DSR;
		cmd.cmdarg = (mmc->dsr & 0xffff) << 16;
		cmd.resp_type = MMC_RSP_NONE;
		if (tegra_mmc_send_cmd(&mPriv, &cmd, NULL))
			printf("MMC: SET_DSR failed\n");
	}

	/* Select the card, and put it into Transfer Mode */
	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);

	/*
	 * For SD, its erase group is always one sector
	 */
	mmc->erase_grp_size = 1;
	mmc->part_config = MMCPART_NOAVAILABLE;
	if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) 
	{
		// Eventually we will implement eMMC ops
		// Leave it empty now
		ASSERT(FALSE);
	}

	// Same for here
	err = mmc_set_capacity(mmc, mBlkDesc.hwpart);
	if (err) goto exit;

	if (IS_SD(mmc))
	{
		err = sd_change_freq(mmc);
	}
	else
	{
		ASSERT(FALSE);
	}

	if (err) goto exit;

	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->cfg->host_caps;
	if (IS_SD(mmc)) 
	{
		if (mmc->card_caps & MMC_MODE_4BIT) 
		{
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = mmc->rca << 16;

			err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
			if (err) goto exit;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
			if (err) goto exit;

			mmc_set_bus_width(mmc, 4);
		}

		err = sd_read_ssr(mmc);
		if (err) goto exit;

		if (mmc->card_caps & MMC_MODE_HS)
		{
			mmc->tran_speed = 50000000;
		}
		else
		{
			mmc->tran_speed = 25000000;
		}
	} 
	else if (mmc->version >= MMC_VERSION_4) 
	{
		ASSERT(FALSE);
	}

	mmc_set_clock(mmc, mmc->tran_speed);

	/* Fix the block length for DDR mode */
	if (mmc->ddr_mode) 
	{
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;
	}

	/* fill in device description */
	struct blk_desc* bdesc = &mBlkDesc;
	bdesc->lun = 0;
	bdesc->hwpart = 0;
	bdesc->type = 0;
	bdesc->blksz = mmc->read_bl_len;
	bdesc->log2blksz = LOG2(bdesc->blksz);
	bdesc->lba = lldiv(mmc->capacity, mmc->read_bl_len);

	/* For Debug purpose */
	DEBUG((EFI_D_INFO, "MMC/SD Block Size: %d \n", bdesc->blksz));
	DEBUG((EFI_D_INFO, "MMC/SD LBA: %lld \n", bdesc->lba));

exit:
	ASSERT(err == 0);
	return err;
}

int mmc_switch_part(struct mmc *mmc, unsigned int part_num)
{
	int ret;

	ret = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONF,
			 (mmc->part_config & ~PART_ACCESS_MASK)
			 | (part_num & PART_ACCESS_MASK));

	/*
	 * Set the capacity if the switch succeeded or was intended
	 * to return to representing the raw device.
	 */
	if ((ret == 0) || ((ret == -ENODEV) && (part_num == 0))) {
		ret = mmc_set_capacity(mmc, part_num);
		mBlkDesc.hwpart = part_num;
	}

	return ret;
}

static int mmc_select_hwpart(int hwpart)
{
	struct blk_desc *desc = &mBlkDesc;
	if (desc->hwpart == hwpart) return 0;

	if (mMmcInstance.part_config == MMCPART_NOAVAILABLE)
		return -EMEDIUMTYPE;

	return mmc_switch_part(&mMmcInstance, hwpart);
}

int mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;

	if (mmc->ddr_mode) return 0;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;

	return tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
}

static int mmc_read_blocks(
	struct mmc *mmc, void *dst, 
	lbaint_t start, lbaint_t blkcnt
)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	if (tegra_mmc_send_cmd(&mPriv, &cmd, &data)) return 0;

	if (blkcnt > 1) 
	{
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (tegra_mmc_send_cmd(&mPriv, &cmd, NULL)) 
		{
			printf("mmc fail to send stop cmd\n");
			return 0;
		}
	}

	return blkcnt;
}

ulong mmc_bread(UINT64 start, UINT64 blkcnt, void *dst)
{
	struct mmc *mmc = &mMmcInstance;
	struct blk_desc *block_dev = &mBlkDesc;
	int err;
	lbaint_t cur, blocks_todo = blkcnt;

	err = mmc_select_hwpart(block_dev->hwpart);
	if (err) goto exit;

	if ((start + blkcnt) > block_dev->lba) 
	{
		DEBUG((EFI_D_ERROR, "MMC: block number 0x%llx exceeds max(0x%llx)\n",
			start + blkcnt, block_dev->lba));
		return 0;
	}

	if (mmc_set_blocklen(mmc, mmc->read_bl_len)) 
	{
		DEBUG((EFI_D_ERROR, "%s: Failed to set blocklen\n", __func__));
		return 0;
	}

	do {
		cur = (blocks_todo > mmc->cfg->b_max) ?
			mmc->cfg->b_max : blocks_todo;
		if (mmc_read_blocks(mmc, dst, start, cur) != cur) 
		{
			DEBUG((EFI_D_ERROR, "%s: Failed to read blocks\n", __func__));
			return 0;
		}
		blocks_todo -= cur;
		start += cur;
		dst += cur * mmc->read_bl_len;
	} 
	while (blocks_todo > 0);

	return blkcnt;
exit:
	return err;
}

EFIAPI
int
SdFxInit(
    VOID
)
{
    int err;

    mmc_set_bus_width(&mMmcInstance, 1);
    mmc_set_clock(&mMmcInstance, 1);

    /* Reset the Card */
	err = mmc_go_idle();

    if (err)
    {
        DEBUG((EFI_D_ERROR, "Failed to reset the card \n"));
    }

    /* The internal partition reset to user partition(0) at every CMD0*/

    /* Test for SD version 2 */
	err = mmc_send_if_cond();

    /* Now try to get the SD card's operating condition */
	err = sd_send_op_cond();

    if (err == -ETIMEDOUT)
    {
		/* If the command timed out, we check for an MMC card */
        err = mmc_send_op_cond(&mMmcInstance);

		if (err) 
		{
			printf("Card did not respond to voltage select!\n");
			return -EOPNOTSUPP;
		}
    }

	if (!err) mMmcInstance.init_in_progress = 1;
	return err;
}

EFIAPI
int
SdFxInitFinalize
(
	VOID
)
{
	int err = 0;
	struct mmc* mmc = &mMmcInstance;

	mmc->init_in_progress = 0;
	if (mmc->op_cond_pending)
		err = mmc_complete_op_cond(mmc);

	if (!err)
		err = mmc_startup(mmc);

	if (err)
		mmc->has_init = 0;
	else
		mmc->has_init = 1;

	return err;
}
