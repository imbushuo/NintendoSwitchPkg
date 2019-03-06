/*
 * (C) Copyright 2013-2015
 * NVIDIA Corporation <www.nvidia.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _TEGRA210_SYSCTR_H_
#define _TEGRA210_SYSCTR_H_

#include <PiDxe.h>
#include <Uefi.h>

#define CONFIG_TEGRA210

struct sysctr_ctlr {
	UINT32 cntcr;		/* 0x00: SYSCTR0_CNTCR Counter Control */
	UINT32 cntsr;		/* 0x04: SYSCTR0_CNTSR Counter Status */
	UINT32 cntcv0;		/* 0x08: SYSCTR0_CNTCV0 Counter Count 31:00 */
	UINT32 cntcv1;		/* 0x0C: SYSCTR0_CNTCV1 Counter Count 63:32 */
	UINT32 reserved1[4];	/* 0x10 - 0x1C */
	UINT32 cntfid0;		/* 0x20: SYSCTR0_CNTFID0 Freq Table Entry */
	UINT32 cntfid1;		/* 0x24: SYSCTR0_CNTFID1 Freq Table End */
	UINT32 reserved2[1002];	/* 0x28 - 0xFCC */
	UINT32 counterid[12];	/* 0xFD0 - 0xFxx CounterID regs, RO */
};

#define TSC_CNTCR_ENABLE	(1 << 0)	/* Enable */
#define TSC_CNTCR_HDBG		(1 << 1)	/* Halt on debug */

#endif	/* _TEGRA210_SYSCTR_H_ */
