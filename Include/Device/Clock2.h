/*
 * (C) Copyright 2013-2015
 * NVIDIA Corporation <www.nvidia.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

/* Tegra210 clock PLL tables */

#ifndef _TEGRA210_CLOCK_TABLES_H_
#define _TEGRA210_CLOCK_TABLES_H_

enum pll_out_id {
	PLL_OUT1,
	PLL_OUT2,
	PLL_OUT3,
	PLL_OUT4
};

/*
 * Clock peripheral IDs which sadly don't match up with PERIPH_ID. we want
 * callers to use the PERIPH_ID for all access to peripheral clocks to avoid
 * confusion bewteen PERIPH_ID_... and PERIPHC_...
 *
 * We don't call this CLOCK_PERIPH_ID or PERIPH_CLOCK_ID as it would just be
 * confusing.
 */
enum periphc_internal_id {
	/* 0x00 */
	PERIPHC_I2S2,
	PERIPHC_I2S3,
	PERIPHC_SPDIF_OUT,
	PERIPHC_SPDIF_IN,
	PERIPHC_PWM,
	PERIPHC_05h,
	PERIPHC_SBC2,
	PERIPHC_SBC3,

	/* 0x08 */
	PERIPHC_08h,
	PERIPHC_I2C1,
	PERIPHC_I2C5,
	PERIPHC_0bh,
	PERIPHC_0ch,
	PERIPHC_SBC1,
	PERIPHC_DISP1,
	PERIPHC_DISP2,

	/* 0x10 */
	PERIPHC_10h,
	PERIPHC_11h,
	PERIPHC_VI,
	PERIPHC_13h,
	PERIPHC_SDMMC1,
	PERIPHC_SDMMC2,
	PERIPHC_G3D,
	PERIPHC_G2D,

	/* 0x18 */
	PERIPHC_18h,
	PERIPHC_SDMMC4,
	PERIPHC_VFIR,
	PERIPHC_1Bh,
	PERIPHC_1Ch,
	PERIPHC_HSI,
	PERIPHC_UART1,
	PERIPHC_UART2,

	/* 0x20 */
	PERIPHC_HOST1X,
	PERIPHC_21h,
	PERIPHC_22h,
	PERIPHC_HDMI,
	PERIPHC_24h,
	PERIPHC_25h,
	PERIPHC_I2C2,
	PERIPHC_EMC,

	/* 0x28 */
	PERIPHC_UART3,
	PERIPHC_29h,
	PERIPHC_VI_SENSOR,
	PERIPHC_2bh,
	PERIPHC_2ch,
	PERIPHC_SBC4,
	PERIPHC_I2C3,
	PERIPHC_SDMMC3,

	/* 0x30 */
	PERIPHC_UART4,
	PERIPHC_UART5,
	PERIPHC_VDE,
	PERIPHC_OWR,
	PERIPHC_NOR,
	PERIPHC_CSITE,
	PERIPHC_I2S1,
	PERIPHC_DTV,

	/* 0x38 */
	PERIPHC_38h,
	PERIPHC_39h,
	PERIPHC_3ah,
	PERIPHC_3bh,
	PERIPHC_MSENC,
	PERIPHC_TSEC,
	PERIPHC_3eh,
	PERIPHC_OSC,

	PERIPHC_VW_FIRST,
	/* 0x40 */
	PERIPHC_40h = PERIPHC_VW_FIRST,
	PERIPHC_MSELECT,
	PERIPHC_TSENSOR,
	PERIPHC_I2S4,
	PERIPHC_I2S5,
	PERIPHC_I2C4,
	PERIPHC_SBC5,
	PERIPHC_SBC6,

	/* 0x48 */
	PERIPHC_AUDIO,
	PERIPHC_49h,
	PERIPHC_4ah,
	PERIPHC_4bh,
	PERIPHC_4ch,
	PERIPHC_HDA2CODEC2X,
	PERIPHC_ACTMON,
	PERIPHC_EXTPERIPH1,

	/* 0x50 */
	PERIPHC_EXTPERIPH2,
	PERIPHC_EXTPERIPH3,
	PERIPHC_52h,
	PERIPHC_I2CSLOW,
	PERIPHC_SYS,
	PERIPHC_55h,
	PERIPHC_56h,
	PERIPHC_57h,

	/* 0x58 */
	PERIPHC_58h,
	PERIPHC_59h,
	PERIPHC_5ah,
	PERIPHC_5bh,
	PERIPHC_SATAOOB,
	PERIPHC_SATA,
	PERIPHC_HDA,		/* 0x428 */
	PERIPHC_5fh,

	PERIPHC_X_FIRST,
	/* 0x60 */
	PERIPHC_XUSB_CORE_HOST = PERIPHC_X_FIRST,	/* 0x600 */
	PERIPHC_XUSB_FALCON,
	PERIPHC_XUSB_FS,
	PERIPHC_XUSB_CORE_DEV,
	PERIPHC_XUSB_SS,
	PERIPHC_CILAB,
	PERIPHC_CILCD,
	PERIPHC_CILE,

	/* 0x68 */
	PERIPHC_DSIA_LP,
	PERIPHC_DSIB_LP,
	PERIPHC_ENTROPY,
	PERIPHC_DVFS_REF,
	PERIPHC_DVFS_SOC,
	PERIPHC_TRACECLKIN,
	PERIPHC_6Eh,
	PERIPHC_6Fh,

	/* 0x70 */
	PERIPHC_EMC_LATENCY,
	PERIPHC_SOC_THERM,
	PERIPHC_72h,
	PERIPHC_73h,
	PERIPHC_74h,
	PERIPHC_75h,
	PERIPHC_VI_SENSOR2,
	PERIPHC_I2C6,

	/* 0x78 */
	PERIPHC_78h,
	PERIPHC_EMC_DLL,
	PERIPHC_7ah,
	PERIPHC_CLK72MHZ,
	PERIPHC_7ch,
	PERIPHC_7dh,
	PERIPHC_VIC,
	PERIPHC_7fh,

	PERIPHC_Y_FIRST,
	/* 0x80 */
	PERIPHC_SDMMC_LEGACY_TM = PERIPHC_Y_FIRST,	/* 0x694 */
	PERIPHC_NVDEC,			/* 0x698 */
	PERIPHC_NVJPG,			/* 0x69c */
	PERIPHC_NVENC,			/* 0x6a0 */
	PERIPHC_84h,
	PERIPHC_85h,
	PERIPHC_86h,
	PERIPHC_87h,

	/* 0x88 */
	PERIPHC_88h,
	PERIPHC_89h,
	PERIPHC_DMIC3,			/* 0x6bc:  */
	PERIPHC_APE,			/* 0x6c0:  */
	PERIPHC_QSPI,			/* 0x6c4:  */
	PERIPHC_VI_I2C,			/* 0x6c8:  */
	PERIPHC_USB2_HSIC_TRK,		/* 0x6cc:  */
	PERIPHC_PEX_SATA_USB_RX_BYP,	/* 0x6d0:  */

	/* 0x90 */
	PERIPHC_MAUD,			/* 0x6d4:  */
	PERIPHC_TSECB,			/* 0x6d8:  */

	PERIPHC_COUNT,
	PERIPHC_NONE = -1,
};

/* Converts a clock number to a clock register: 0=L, 1=H, 2=U, 0=V, 1=W */
#define PERIPH_REG(id) \
	(id < PERIPH_ID_VW_FIRST) ? \
		((id) >> 5) : ((id - PERIPH_ID_VW_FIRST) >> 5)

/* Mask value for a clock (within PERIPH_REG(id)) */
#define PERIPH_MASK(id) (1 << ((id) & 0x1f))

/* return 1 if a PLL ID is in range */
#define clock_id_is_pll(id) ((id) >= CLOCK_ID_FIRST && (id) < CLOCK_ID_COUNT)

/* return 1 if a peripheral ID is in range */
#define clock_periph_id_isvalid(id) ((id) >= PERIPH_ID_FIRST && \
		(id) < PERIPH_ID_COUNT)

/* Set of oscillator frequencies supported in the internal API. */
enum clock_osc_freq {
	/* All in MHz, so 13_0 is 13.0MHz */
	CLOCK_OSC_FREQ_13_0,
	CLOCK_OSC_FREQ_19_2,
	CLOCK_OSC_FREQ_12_0,
	CLOCK_OSC_FREQ_26_0,
	CLOCK_OSC_FREQ_38_4,
	CLOCK_OSC_FREQ_48_0,

	CLOCK_OSC_FREQ_COUNT,
};

/*
 * Note that no Tegra clock register actually uses all of bits 31:28 as
 * the mux field. Rather, bits 30:28, 29:28, or 28 are used. However, in
 * those cases, nothing is stored in the bits about the mux field, so it's
 * safe to pretend that the mux field extends all the way to the end of the
 * register. As such, the U-Boot clock driver is currently a bit lazy, and
 * doesn't distinguish between 31:28, 30:28, 29:28 and 28; it just lumps
 * them all together and pretends they're all 31:28.
 */
enum {
	MASK_BITS_31_30,
	MASK_BITS_31_29,
	MASK_BITS_31_28,
};

struct clk_pll_info {
	UINT32	m_shift:5;	/* DIVM_SHIFT */
	UINT32	n_shift:5;	/* DIVN_SHIFT */
	UINT32	p_shift:5;	/* DIVP_SHIFT */
	UINT32	kcp_shift:5;	/* KCP/cpcon SHIFT */
	UINT32	kvco_shift:5;	/* KVCO/lfcon SHIFT */
	UINT32	lock_ena:6;	/* LOCK_ENABLE/EN_LOCKDET shift */
	UINT32	rsvd:1;
	UINT32	m_mask:10;	/* DIVM_MASK */
	UINT32	n_mask:12;	/* DIVN_MASK */
	UINT32	p_mask:10;	/* DIVP_MASK or VCO_MASK */
	UINT32	kcp_mask:10;	/* KCP/CPCON MASK */
	UINT32	kvco_mask:10;	/* KVCO/LFCON MASK */
	UINT32	lock_det:6;	/* LOCK_DETECT/LOCKED shift */
	UINT32	rsvd2:6;
};

/* Number of PLL-based clocks (i.e. not OSC, MCLK or 32KHz) */
#define CLOCK_ID_PLL_COUNT	(CLOCK_ID_COUNT - 3)

/* return 1 if a periphc_internal_id is in range */
#define periphc_internal_id_isvalid(id) ((id) >= 0 && \
		(id) < PERIPHC_COUNT)

/* return 1 if a peripheral ID is in range */
#define clock_type_id_isvalid(id) ((id) >= 0 && \
		(id) < CLOCK_TYPE_COUNT)

#define NV_PA_CLK_RST_BASE	0x60006000

/* CLK_RST_CONTROLLER_PLLx_BASE_0 */
#define PLL_BYPASS_SHIFT	31
#define PLL_BYPASS_MASK		(1U << PLL_BYPASS_SHIFT)

#define PLL_ENABLE_SHIFT	30
#define PLL_ENABLE_MASK		(1U << PLL_ENABLE_SHIFT)

#define PLL_BASE_OVRRIDE_MASK	(1U << 28)

#define PLL_LOCK_SHIFT		27
#define PLL_LOCK_MASK		(1U << PLL_LOCK_SHIFT)

/*
 * Most PLLs use the clk_pll structure, but some have a simpler two-member
 * structure for which we use clk_pll_simple. The reason for this non-
 * othogonal setup is not stated.
 */
enum {
	TEGRA_CLK_PLLS		= 6,	/* Number of normal PLLs */
	TEGRA_CLK_SIMPLE_PLLS	= 3,	/* Number of simple PLLs */
	TEGRA_CLK_REGS		= 3,	/* Number of clock enable regs L/H/U */
	TEGRA_CLK_SOURCES	= 64,	/* Number of ppl clock sources L/H/U */
	TEGRA_CLK_REGS_VW	= 2,	/* Number of clock enable regs V/W */
	TEGRA_CLK_SOURCES_VW	= 32,	/* Number of ppl clock sources V/W */
	TEGRA_CLK_SOURCES_X	= 32,	/* Number of ppl clock sources X */
	TEGRA_CLK_SOURCES_Y	= 18,	/* Number of ppl clock sources Y */
};

/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll {
	unsigned int pll_base;		/* the control register */
	/* pll_out[0] is output A control, pll_out[1] is output B control */
	unsigned int pll_out[2];
	unsigned int pll_misc;		/* other misc things */
};

/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll_simple {
	unsigned int pll_base;		/* the control register */
	unsigned int pll_misc;		/* other misc things */
};

struct clk_pllm {
	unsigned int pllm_base;		/* the control register */
	unsigned int pllm_out;		/* output control */
	unsigned int pllm_misc1;	/* misc1 */
	unsigned int pllm_misc2;	/* misc2 */
};

/* RST_DEV_(L,H,U,V,W)_(SET,CLR) and CLK_ENB_(L,H,U,V,W)_(SET,CLR) */
struct clk_set_clr {
	unsigned int set;
	unsigned int clr;
};

/* Clock/Reset Controller (CLK_RST_CONTROLLER_) regs */
struct clk_rst_ctlr {
	unsigned int crc_rst_src;			/* _RST_SOURCE_0,0x00 */
	unsigned int crc_rst_dev[TEGRA_CLK_REGS];	/* _RST_DEVICES_L/H/U_0 */
	unsigned int crc_clk_out_enb[TEGRA_CLK_REGS];	/* _CLK_OUT_ENB_L/H/U_0 */
	unsigned int crc_reserved0;		/* reserved_0,		0x1C */
	unsigned int crc_cclk_brst_pol;		/* _CCLK_BURST_POLICY_0, 0x20 */
	unsigned int crc_super_cclk_div;	/* _SUPER_CCLK_DIVIDER_0,0x24 */
	unsigned int crc_sclk_brst_pol;		/* _SCLK_BURST_POLICY_0, 0x28 */
	unsigned int crc_super_sclk_div;	/* _SUPER_SCLK_DIVIDER_0,0x2C */
	unsigned int crc_clk_sys_rate;		/* _CLK_SYSTEM_RATE_0,	0x30 */
	unsigned int crc_prog_dly_clk;		/* _PROG_DLY_CLK_0,	0x34 */
	unsigned int crc_aud_sync_clk_rate;	/* _AUDIO_SYNC_CLK_RATE_0,0x38 */
	unsigned int crc_reserved1;		/* reserved_1,		0x3C */
	unsigned int crc_cop_clk_skip_plcy;	/* _COP_CLK_SKIP_POLICY_0,0x40 */
	unsigned int crc_clk_mask_arm;		/* _CLK_MASK_ARM_0,	0x44 */
	unsigned int crc_misc_clk_enb;		/* _MISC_CLK_ENB_0,	0x48 */
	unsigned int crc_clk_cpu_cmplx;		/* _CLK_CPU_CMPLX_0,	0x4C */
	unsigned int crc_osc_ctrl;		/* _OSC_CTRL_0,		0x50 */
	unsigned int crc_pll_lfsr;		/* _PLL_LFSR_0,		0x54 */
	unsigned int crc_osc_freq_det;		/* _OSC_FREQ_DET_0,	0x58 */
	unsigned int crc_osc_freq_det_stat;	/* _OSC_FREQ_DET_STATUS_0,0x5C */
	unsigned int crc_reserved2[8];		/* reserved_2[8],	0x60-7C */

	struct clk_pll crc_pll[TEGRA_CLK_PLLS];	/* PLLs from 0x80 to 0xdc */

	/* PLLs from 0xe0 to 0xf4    */
	struct clk_pll_simple crc_pll_simple[TEGRA_CLK_SIMPLE_PLLS];

	unsigned int crc_reserved10;		/* _reserved_10,	0xF8 */
	unsigned int crc_reserved11;		/* _reserved_11,	0xFC */

	unsigned int crc_clk_src[TEGRA_CLK_SOURCES]; /*_I2S1_0...	0x100-1fc */

	unsigned int crc_reserved20[32];	/* _reserved_20,	0x200-27c */

	unsigned int crc_clk_out_enb_x;		/* _CLK_OUT_ENB_X_0,	0x280 */
	unsigned int crc_clk_enb_x_set;		/* _CLK_ENB_X_SET_0,	0x284 */
	unsigned int crc_clk_enb_x_clr;		/* _CLK_ENB_X_CLR_0,	0x288 */

	unsigned int crc_rst_devices_x;		/* _RST_DEVICES_X_0,	0x28c */
	unsigned int crc_rst_dev_x_set;		/* _RST_DEV_X_SET_0,	0x290 */
	unsigned int crc_rst_dev_x_clr;		/* _RST_DEV_X_CLR_0,	0x294 */

	unsigned int crc_clk_out_enb_y;		/* _CLK_OUT_ENB_Y_0,	0x298 */
	unsigned int crc_clk_enb_y_set;		/* _CLK_ENB_Y_SET_0,	0x29c */
	unsigned int crc_clk_enb_y_clr;		/* _CLK_ENB_Y_CLR_0,	0x2a0 */

	unsigned int crc_rst_devices_y;		/* _RST_DEVICES_Y_0,	0x2a4 */
	unsigned int crc_rst_dev_y_set;		/* _RST_DEV_Y_SET_0,	0x2a8 */
	unsigned int crc_rst_dev_y_clr;		/* _RST_DEV_Y_CLR_0,	0x2ac */

	unsigned int crc_reserved21[17];	/* _reserved_21,	0x2b0-2f0 */

	unsigned int crc_dfll_base;		/* _DFLL_BASE_0,	0x2f4 */

	unsigned int crc_reserved22[2];		/* _reserved_22,	0x2f8-2fc */

	/* _RST_DEV_L/H/U_SET_0 0x300 ~ 0x314 */
	struct clk_set_clr crc_rst_dev_ex[TEGRA_CLK_REGS];

	unsigned int crc_reserved30[2];		/* _reserved_30,	0x318, 0x31c */

	/* _CLK_ENB_L/H/U_CLR_0 0x320 ~ 0x334 */
	struct clk_set_clr crc_clk_enb_ex[TEGRA_CLK_REGS];

	unsigned int crc_reserved31[2];		/* _reserved_31,	0x338, 0x33c */

	unsigned int crc_cpu_cmplx_set;		/* _RST_CPU_CMPLX_SET_0,    0x340 */
	unsigned int crc_cpu_cmplx_clr;		/* _RST_CPU_CMPLX_CLR_0,    0x344 */

	/* Additional (T30) registers */
	unsigned int crc_clk_cpu_cmplx_set;	/* _CLK_CPU_CMPLX_SET_0,    0x348 */
	unsigned int crc_clk_cpu_cmplx_clr;	/* _CLK_CPU_CMPLX_SET_0,    0x34c */

	unsigned int crc_reserved32[2];		/* _reserved_32,      0x350,0x354 */

	unsigned int crc_rst_dev_vw[TEGRA_CLK_REGS_VW]; /* _RST_DEVICES_V/W_0 */
	unsigned int crc_clk_out_enb_vw[TEGRA_CLK_REGS_VW]; /* _CLK_OUT_ENB_V/W_0 */
	unsigned int crc_cclkg_brst_pol;	/* _CCLKG_BURST_POLICY_0,   0x368 */
	unsigned int crc_super_cclkg_div;	/* _SUPER_CCLKG_DIVIDER_0,  0x36C */
	unsigned int crc_cclklp_brst_pol;	/* _CCLKLP_BURST_POLICY_0,  0x370 */
	unsigned int crc_super_cclkp_div;	/* _SUPER_CCLKLP_DIVIDER_0, 0x374 */
	unsigned int crc_clk_cpug_cmplx;	/* _CLK_CPUG_CMPLX_0,       0x378 */
	unsigned int crc_clk_cpulp_cmplx;	/* _CLK_CPULP_CMPLX_0,      0x37C */
	unsigned int crc_cpu_softrst_ctrl;	/* _CPU_SOFTRST_CTRL_0,     0x380 */
	unsigned int crc_cpu_softrst_ctrl1;	/* _CPU_SOFTRST_CTRL1_0,    0x384 */
	unsigned int crc_cpu_softrst_ctrl2;	/* _CPU_SOFTRST_CTRL2_0,    0x388 */
	unsigned int crc_reserved33[9];		/* _reserved_33,        0x38c-3ac */
	unsigned int crc_clk_src_vw[TEGRA_CLK_SOURCES_VW];	/* 0x3B0-0x42C */
	/* _RST_DEV_V/W_SET_0 0x430 ~ 0x43c */
	struct clk_set_clr crc_rst_dev_ex_vw[TEGRA_CLK_REGS_VW];
	/* _CLK_ENB_V/W_CLR_0 0x440 ~ 0x44c */
	struct clk_set_clr crc_clk_enb_ex_vw[TEGRA_CLK_REGS_VW];
	/* Additional (T114+) registers */
	unsigned int crc_rst_cpug_cmplx_set;	/* _RST_CPUG_CMPLX_SET_0,  0x450 */
	unsigned int crc_rst_cpug_cmplx_clr;	/* _RST_CPUG_CMPLX_CLR_0,  0x454 */
	unsigned int crc_rst_cpulp_cmplx_set;	/* _RST_CPULP_CMPLX_SET_0, 0x458 */
	unsigned int crc_rst_cpulp_cmplx_clr;	/* _RST_CPULP_CMPLX_CLR_0, 0x45C */
	unsigned int crc_clk_cpug_cmplx_set;	/* _CLK_CPUG_CMPLX_SET_0,  0x460 */
	unsigned int crc_clk_cpug_cmplx_clr;	/* _CLK_CPUG_CMPLX_CLR_0,  0x464 */
	unsigned int crc_clk_cpulp_cmplx_set;	/* _CLK_CPULP_CMPLX_SET_0, 0x468 */
	unsigned int crc_clk_cpulp_cmplx_clr;	/* _CLK_CPULP_CMPLX_CLR_0, 0x46C */
	unsigned int crc_cpu_cmplx_status;	/* _CPU_CMPLX_STATUS_0,    0x470 */
	unsigned int crc_reserved40[1];		/* _reserved_40,        0x474 */
	unsigned int crc_intstatus;		/* __INTSTATUS_0,       0x478 */
	unsigned int crc_intmask;		/* __INTMASK_0,         0x47C */
	unsigned int crc_utmip_pll_cfg0;	/* _UTMIP_PLL_CFG0_0,	0x480 */
	unsigned int crc_utmip_pll_cfg1;	/* _UTMIP_PLL_CFG1_0,	0x484 */
	unsigned int crc_utmip_pll_cfg2;	/* _UTMIP_PLL_CFG2_0,	0x488 */

	unsigned int crc_plle_aux;		/* _PLLE_AUX_0,		0x48C */
	unsigned int crc_sata_pll_cfg0;		/* _SATA_PLL_CFG0_0,	0x490 */
	unsigned int crc_sata_pll_cfg1;		/* _SATA_PLL_CFG1_0,	0x494 */
	unsigned int crc_pcie_pll_cfg0;		/* _PCIE_PLL_CFG0_0,	0x498 */

	unsigned int crc_prog_audio_dly_clk;	/* _PROG_AUDIO_DLY_CLK_0, 0x49C */
	unsigned int crc_audio_sync_clk_i2s0;	/* _AUDIO_SYNC_CLK_I2S0_0, 0x4A0 */
	unsigned int crc_audio_sync_clk_i2s1;	/* _AUDIO_SYNC_CLK_I2S1_0, 0x4A4 */
	unsigned int crc_audio_sync_clk_i2s2;	/* _AUDIO_SYNC_CLK_I2S2_0, 0x4A8 */
	unsigned int crc_audio_sync_clk_i2s3;	/* _AUDIO_SYNC_CLK_I2S3_0, 0x4AC */
	unsigned int crc_audio_sync_clk_i2s4;	/* _AUDIO_SYNC_CLK_I2S4_0, 0x4B0 */
	unsigned int crc_audio_sync_clk_spdif;	/* _AUDIO_SYNC_CLK_SPDIF_0, 0x4B4 */

	unsigned int crc_plld2_base;		/* _PLLD2_BASE_0, 0x4B8 */
	unsigned int crc_plld2_misc;		/* _PLLD2_MISC_0, 0x4BC */
	unsigned int crc_utmip_pll_cfg3;	/* _UTMIP_PLL_CFG3_0, 0x4C0 */
	unsigned int crc_pllrefe_base;		/* _PLLREFE_BASE_0, 0x4C4 */
	unsigned int crc_pllrefe_misc;		/* _PLLREFE_MISC_0, 0x4C8 */
	unsigned int crs_reserved_50[7];	/* _reserved_50, 0x4CC-0x4E4 */
	unsigned int crc_pllc2_base;		/* _PLLC2_BASE_0, 0x4E8 */
	unsigned int crc_pllc2_misc0;		/* _PLLC2_MISC_0_0, 0x4EC */
	unsigned int crc_pllc2_misc1;		/* _PLLC2_MISC_1_0, 0x4F0 */
	unsigned int crc_pllc2_misc2;		/* _PLLC2_MISC_2_0, 0x4F4 */
	unsigned int crc_pllc2_misc3;		/* _PLLC2_MISC_3_0, 0x4F8 */
	unsigned int crc_pllc3_base;		/* _PLLC3_BASE_0, 0x4FC */
	unsigned int crc_pllc3_misc0;		/* _PLLC3_MISC_0_0, 0x500 */
	unsigned int crc_pllc3_misc1;		/* _PLLC3_MISC_1_0, 0x504 */
	unsigned int crc_pllc3_misc2;		/* _PLLC3_MISC_2_0, 0x508 */
	unsigned int crc_pllc3_misc3;		/* _PLLC3_MISC_3_0, 0x50C */
	unsigned int crc_pllx_misc1;		/* _PLLX_MISC_1_0, 0x510 */
	unsigned int crc_pllx_misc2;		/* _PLLX_MISC_2_0, 0x514 */
	unsigned int crc_pllx_misc3;		/* _PLLX_MISC_3_0, 0x518 */
	unsigned int crc_xusbio_pll_cfg0;	/* _XUSBIO_PLL_CFG0_0, 0x51C */
	unsigned int crc_xusbio_pll_cfg1;	/* _XUSBIO_PLL_CFG0_1, 0x520 */
	unsigned int crc_plle_aux1;		/* _PLLE_AUX1_0, 0x524 */
	unsigned int crc_pllp_reshift;		/* _PLLP_RESHIFT_0, 0x528 */
	unsigned int crc_utmipll_hw_pwrdn_cfg0;	/* _UTMIPLL_HW_PWRDN_CFG0_0, 0x52C */
	unsigned int crc_pllu_hw_pwrdn_cfg0;	/* _PLLU_HW_PWRDN_CFG0_0, 0x530 */
	unsigned int crc_xusb_pll_cfg0;		/* _XUSB_PLL_CFG0_0, 0x534 */
	unsigned int crc_reserved51[1];		/* _reserved_51, 0x538 */
	unsigned int crc_clk_cpu_misc;		/* _CLK_CPU_MISC_0, 0x53C */
	unsigned int crc_clk_cpug_misc;		/* _CLK_CPUG_MISC_0, 0x540 */
	unsigned int crc_clk_cpulp_misc;	/* _CLK_CPULP_MISC_0, 0x544 */
	unsigned int crc_pllx_hw_ctrl_cfg;	/* _PLLX_HW_CTRL_CFG_0, 0x548 */
	unsigned int crc_pllx_sw_ramp_cfg;	/* _PLLX_SW_RAMP_CFG_0, 0x54C */
	unsigned int crc_pllx_hw_ctrl_status;	/* _PLLX_HW_CTRL_STATUS_0, 0x550 */
	unsigned int crc_reserved52[1];		/* _reserved_52, 0x554 */
	unsigned int crc_super_gr3d_clk_div;	/* _SUPER_GR3D_CLK_DIVIDER_0, 0x558 */
	unsigned int crc_spare_reg0;		/* _SPARE_REG0_0, 0x55C */
	UINT32 _rsv32[4];                  /*                    0x560-0x56c */
	UINT32 crc_plld2_ss_cfg;		/* _PLLD2_SS_CFG            0x570 */
	UINT32 _rsv32_1[7];		/*                      0x574-58c */
	struct clk_pll_simple plldp;	/* _PLLDP_BASE, 0x590 _PLLDP_MISC */
	UINT32 crc_plldp_ss_cfg;		/* _PLLDP_SS_CFG, 0x598 */

	/* Tegra124+ - skip to 0x600 here for new CLK_SOURCE_ regs */
	unsigned int _rsrv32_2[25];			/* _0x59C - 0x5FC */
	unsigned int crc_clk_src_x[TEGRA_CLK_SOURCES_X]; /* XUSB, etc, 0x600-0x67C */

	/* Tegra210 - skip to 0x694 here for new CLK_SOURCE_ regs */
	unsigned int crc_reserved61[5];	/* _reserved_61, 0x680 - 0x690 */
	/*
	 * NOTE: PLLA1 regs are in the middle of this Y region. Break this in
	 * two later if PLLA1 is needed, but for now this is cleaner.
	 */
	unsigned int crc_clk_src_y[TEGRA_CLK_SOURCES_Y]; /* SPARE1, etc, 0x694-0x6D8 */
};

/*
 * CLK_RST_CONTROLLER_CLK_SOURCE_x_OUT_0 - the mask here is normally 8 bits
 * but can be 16. We could use knowledge we have to restrict the mask in
 * the 8-bit cases (the divider_bits value returned by
 * get_periph_clock_source()) but it does not seem worth it since the code
 * already checks the ranges of values it is writing, in clk_get_divider().
 */
#define OUT_CLK_DIVISOR_SHIFT	0
#define OUT_CLK_DIVISOR_MASK	(0xffff << OUT_CLK_DIVISOR_SHIFT)

#define OUT_CLK_SOURCE_31_30_SHIFT	30
#define OUT_CLK_SOURCE_31_30_MASK	(3U << OUT_CLK_SOURCE_31_30_SHIFT)

#define OUT_CLK_SOURCE_31_29_SHIFT	29
#define OUT_CLK_SOURCE_31_29_MASK	(7U << OUT_CLK_SOURCE_31_29_SHIFT)

/* Note: See comment for MASK_BITS_31_28 in arch-tegra/clock.h */
#define OUT_CLK_SOURCE_31_28_SHIFT	28
#define OUT_CLK_SOURCE_31_28_MASK	(15U << OUT_CLK_SOURCE_31_28_SHIFT)

#endif	/* _TEGRA210_CLOCK_TABLES_H_ */
