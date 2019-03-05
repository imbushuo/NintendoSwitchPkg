/*
* Copyright (c) 2018 naehrwert
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __NVIDIA_T210_H__
#define __NVIDIA_T210_H__

#define HOST1X_BASE 0x50000000
#define DISPLAY_A_BASE 0x54200000
#define DSI_BASE 0x54300000
#define VIC_BASE 0x54340000
#define TSEC_BASE 0x54500000
#define SOR1_BASE 0x54580000
#define PG_UP_BASE 0x60000000
#define TMR_BASE 0x60005000
#define RTC_BASE 0x7000E000
#define CLOCK_BASE 0x60006000
#define FLOW_CTLR_BASE 0x60007000
#define SYSREG_BASE 0x6000C000
#define SB_BASE (SYSREG_BASE + 0x200)
#define GPIO_BASE 0x6000D000
#define GPIO_1_BASE (GPIO_BASE)
#define GPIO_2_BASE (GPIO_BASE + 0x100)
#define GPIO_3_BASE (GPIO_BASE + 0x200)
#define GPIO_4_BASE (GPIO_BASE + 0x300)
#define GPIO_5_BASE (GPIO_BASE + 0x400)
#define GPIO_6_BASE (GPIO_BASE + 0x500)
#define GPIO_7_BASE (GPIO_BASE + 0x600)
#define GPIO_8_BASE (GPIO_BASE + 0x700)
#define EXCP_VEC_BASE 0x6000F000
#define APB_MISC_BASE 0x70000000
#define PINMUX_AUX_BASE 0x70003000
#define UART_BASE 0x70006000
#define PMC_BASE 0x7000E400
#define SYSCTR0_BASE 0x7000F000
#define FUSE_BASE 0x7000F800
#define KFUSE_BASE 0x7000FC00
#define SE_BASE 0x70012000
#define MC_BASE 0x70019000
#define EMC_BASE 0x7001B000
#define MIPI_CAL_BASE 0x700E3000
#define I2S_BASE 0x702D1000

#define _REG(base, off) *(vu32 *)((base) + (off))

#define HOST1X(off) _REG(HOST1X_BASE, off)
#define DISPLAY_A(off) _REG(DISPLAY_A_BASE, off)
#define DSI(off) _REG(DSI_BASE, off)
#define VIC(off) _REG(VIC_BASE, off)
#define TSEC(off) _REG(TSEC_BASE, off)
#define SOR1(off) _REG(SOR1_BASE, off)
#define TMR(off) _REG(TMR_BASE, off)
#define CLOCK(off) _REG(CLOCK_BASE, off)
#define FLOW_CTLR(off) _REG(FLOW_CTLR_BASE, off)
#define SYSREG(off) _REG(SYSREG_BASE, off)
#define SB(off) _REG(SB_BASE, off)
#define GPIO(off) _REG(GPIO_BASE, off)
#define GPIO_1(off) _REG(GPIO_1_BASE, off)
#define GPIO_2(off) _REG(GPIO_2_BASE, off)
#define GPIO_3(off) _REG(GPIO_3_BASE, off)
#define GPIO_4(off) _REG(GPIO_4_BASE, off)
#define GPIO_5(off) _REG(GPIO_5_BASE, off)
#define GPIO_6(off) _REG(GPIO_6_BASE, off)
#define GPIO_7(off) _REG(GPIO_7_BASE, off)
#define GPIO_8(off) _REG(GPIO_8_BASE, off)
#define EXCP_VEC(off) _REG(EXCP_VEC_BASE, off)
#define APB_MISC(off) _REG(APB_MISC_BASE, off)
#define PINMUX_AUX(off) _REG(PINMUX_AUX_BASE, off)
#define PMC(off) _REG(PMC_BASE, off)
#define SYSCTR0(off) _REG(SYSCTR0_BASE, off)
#define FUSE(off) _REG(FUSE_BASE, off)
#define KFUSE(off) _REG(KFUSE_BASE, off)
#define SE(off) _REG(SE_BASE, off)
#define MC(off) _REG(MC_BASE, off)
#define EMC(off) _REG(EMC_BASE, off)
#define MIPI_CAL(off) _REG(MIPI_CAL_BASE, off)
#define I2S(off) _REG(I2S_BASE, off)
#define RTC(off) _REG(RTC_BASE, off)

/*! System registers. */
#define AHB_ARBITRATION_XBAR_CTRL 0xE0

/*! APB MISC registers. */
#define APB_MISC_GP_SDMMC1_CLK_LPBK_CONTROL 0x8D4
#define APB_MISC_GP_SDMMC3_CLK_LPBK_CONTROL 0x8D8
#define APB_MISC_GP_SDMMC1_PAD_CFGPADCTRL 0xA98
#define APB_MISC_GP_VGPIO_GPIO_MUX_SEL 0xB74

/*! Secure boot registers. */
#define SB_CSR 0x0
#define SB_AA64_RESET_LOW 0x30
#define SB_AA64_RESET_HIGH 0x34

/*! SYSCTR0 registers. */
#define SYSCTR0_CNTFID0 0x20

/* System Timer */
#define TMR_US_OFFS     0x10
#define TMR_US_CFG_OFFS 0x14
#define TIMERUS_CNTR_1US (0x10 + 0x0)

/*! RTC registers. */
#define APBDEV_RTC_SECONDS        0x8
#define APBDEV_RTC_SHADOW_SECONDS 0xC
#define APBDEV_RTC_MILLI_SECONDS 0x10

/* Memory controller */
#define NV_PA_MC_BASE		0x70019000	/* Mem Ctlr regs (MCB, etc.) */
#define NV_PA_SDRAM_BASE	0x80000000

/**
 * Defines the memory controller registers we need/care about
 */
struct mc_ctlr {
	u32 reserved0[4];			/* offset 0x00 - 0x0C */
	u32 mc_smmu_config;			/* offset 0x10 */
	u32 mc_smmu_tlb_config;			/* offset 0x14 */
	u32 mc_smmu_ptc_config;			/* offset 0x18 */
	u32 mc_smmu_ptb_asid;			/* offset 0x1C */
	u32 mc_smmu_ptb_data;			/* offset 0x20 */
	u32 reserved1[3];			/* offset 0x24 - 0x2C */
	u32 mc_smmu_tlb_flush;			/* offset 0x30 */
	u32 mc_smmu_ptc_flush;			/* offset 0x34 */
	u32 reserved2[6];			/* offset 0x38 - 0x4C */
	u32 mc_emem_cfg;			/* offset 0x50 */
	u32 mc_emem_adr_cfg;			/* offset 0x54 */
	u32 mc_emem_adr_cfg_dev0;		/* offset 0x58 */
	u32 mc_emem_adr_cfg_dev1;		/* offset 0x5C */
	u32 reserved3[4];			/* offset 0x60 - 0x6C */
	u32 mc_security_cfg0;			/* offset 0x70 */
	u32 mc_security_cfg1;			/* offset 0x74 */
	u32 reserved4[6];			/* offset 0x7C - 0x8C */
	u32 mc_emem_arb_reserved[28];		/* offset 0x90 - 0xFC */
	u32 reserved5[74];			/* offset 0x100 - 0x224 */
	u32 mc_smmu_translation_enable_0;	/* offset 0x228 */
	u32 mc_smmu_translation_enable_1;	/* offset 0x22C */
	u32 mc_smmu_translation_enable_2;	/* offset 0x230 */
	u32 mc_smmu_translation_enable_3;	/* offset 0x234 */
	u32 mc_smmu_afi_asid;			/* offset 0x238 */
	u32 mc_smmu_avpc_asid;			/* offset 0x23C */
	u32 mc_smmu_dc_asid;			/* offset 0x240 */
	u32 mc_smmu_dcb_asid;			/* offset 0x244 */
	u32 reserved6[2];                       /* offset 0x248 - 0x24C */
	u32 mc_smmu_hc_asid;			/* offset 0x250 */
	u32 mc_smmu_hda_asid;			/* offset 0x254 */
	u32 mc_smmu_isp2_asid;			/* offset 0x258 */
	u32 reserved7[2];                       /* offset 0x25C - 0x260 */
	u32 mc_smmu_msenc_asid;			/* offset 0x264 */
	u32 mc_smmu_nv_asid;			/* offset 0x268 */
	u32 mc_smmu_nv2_asid;			/* offset 0x26C */
	u32 mc_smmu_ppcs_asid;			/* offset 0x270 */
	u32 mc_smmu_sata_asid;			/* offset 0x274 */
	u32 reserved8[1];                       /* offset 0x278 */
	u32 mc_smmu_vde_asid;			/* offset 0x27C */
	u32 mc_smmu_vi_asid;			/* offset 0x280 */
	u32 mc_smmu_vic_asid;			/* offset 0x284 */
	u32 mc_smmu_xusb_host_asid;		/* offset 0x288 */
	u32 mc_smmu_xusb_dev_asid;		/* offset 0x28C */
	u32 reserved9[1];                       /* offset 0x290 */
	u32 mc_smmu_tsec_asid;			/* offset 0x294 */
	u32 mc_smmu_ppcs1_asid;			/* offset 0x298 */
	u32 reserved10[235];			/* offset 0x29C - 0x644 */
	u32 mc_video_protect_bom;		/* offset 0x648 */
	u32 mc_video_protect_size_mb;		/* offset 0x64c */
	u32 mc_video_protect_reg_ctrl;		/* offset 0x650 */
	u32 rsvd_0x654[4];			/* 0x654 */
	u32 emem_cfg_access_ctrl;		/* 0x664 */
	u32 rsvd_0x668[2];			/* 0x668 */
	u32 sec_carveout_bom;		/* 0x670 */
	u32 sec_carveout_size_mb;		/* 0x674 */
	u32 sec_carveout_reg_ctrl;		/* 0x678 */
	u32 rsvd_0x67c[17];		/* 0x67C-0x6BC */

	u32 emem_arb_timing_rfcpb;		/* 0x6C0 */
	u32 emem_arb_timing_ccdmw;		/* 0x6C4 */
	u32 rsvd_0x6c8[10];		/* 0x6C8-0x6EC */

	u32 emem_arb_refpb_hp_ctrl;	/* 0x6F0 */
	u32 emem_arb_refpb_bank_ctrl;	/* 0x6F4 */
	u32 rsvd_0x6f8[156];		/* 0x6F8-0x964 */

	u32 emem_arb_override_1;		/* 0x968 */
	u32 rsvd_0x96c[3];			/* 0x96c */
	u32 video_protect_bom_adr_hi;	/* 0x978 */
	u32 rsvd_0x97c[2];			/* 0x97c */
	u32 video_protect_gpu_override_0;	/* 0x984 */
	u32 video_protect_gpu_override_1;	/* 0x988 */
	u32 rsvd_0x98c[5];			/* 0x98c */
	u32 mts_carveout_bom;		/* 0x9a0 */
	u32 mts_carveout_size_mb;		/* 0x9a4 */
	u32 mts_carveout_adr_hi;		/* 0x9a8 */
	u32 mts_carveout_reg_ctrl;		/* 0x9ac */
	u32 rsvd_0x9b0[4];			/* 0x9b0 */
	u32 emem_bank_swizzle_cfg0;	/* 0x9c0 */
	u32 emem_bank_swizzle_cfg1;	/* 0x9c4 */
	u32 emem_bank_swizzle_cfg2;	/* 0x9c8 */
	u32 emem_bank_swizzle_cfg3;	/* 0x9cc */
	u32 rsvd_0x9d0[1];			/* 0x9d0 */
	u32 sec_carveout_adr_hi;		/* 0x9d4 */
	u32 rsvd_0x9d8;			/* 0x9D8 */
	u32 da_config0;			/* 0x9DC */
	u32 rsvd_0x9c0[138];		/* 0x9E0-0xc04 */

	u32 security_carveout1_cfg0;	/* 0xc08 */
	u32 security_carveout1_bom;	/* 0xc0c */
	u32 security_carveout1_bom_hi;	/* 0xc10 */
	u32 security_carveout1_size_128kb;	/* 0xc14 */
	u32 security_carveout1_ca0;	/* 0xc18 */
	u32 security_carveout1_ca1;	/* 0xc1c */
	u32 security_carveout1_ca2;	/* 0xc20 */
	u32 security_carveout1_ca3;	/* 0xc24 */
	u32 security_carveout1_ca4;	/* 0xc28 */
	u32 security_carveout1_cfia0;	/* 0xc2c */
	u32 security_carveout1_cfia1;	/* 0xc30 */
	u32 security_carveout1_cfia2;	/* 0xc34 */
	u32 security_carveout1_cfia3;	/* 0xc38 */
	u32 security_carveout1_cfia4;	/* 0xc3c */
	u32 rsvd_0xc40[6];			/* 0xc40-0xc54 */

	u32 security_carveout2_cfg0;	/* 0xc58 */
	u32 security_carveout2_bom;	/* 0xc5c */
	u32 security_carveout2_bom_hi;	/* 0xc60 */
	u32 security_carveout2_size_128kb;	/* 0xc64 */
	u32 security_carveout2_ca0;	/* 0xc68 */
	u32 security_carveout2_ca1;	/* 0xc6c */
	u32 security_carveout2_ca2;	/* 0xc70 */
	u32 security_carveout2_ca3;	/* 0xc74 */
	u32 security_carveout2_ca4;	/* 0xc78 */
	u32 security_carveout2_cfia0;	/* 0xc7c */
	u32 security_carveout2_cfia1;	/* 0xc80 */
	u32 security_carveout2_cfia2;	/* 0xc84 */
	u32 security_carveout2_cfia3;	/* 0xc88 */
	u32 security_carveout2_cfia4;	/* 0xc8c */
	u32 rsvd_0xc90[6];			/* 0xc90-0xca4 */

	u32 security_carveout3_cfg0;	/* 0xca8 */
	u32 security_carveout3_bom;	/* 0xcac */
	u32 security_carveout3_bom_hi;	/* 0xcb0 */
	u32 security_carveout3_size_128kb;	/* 0xcb4 */
	u32 security_carveout3_ca0;	/* 0xcb8 */
	u32 security_carveout3_ca1;	/* 0xcbc */
	u32 security_carveout3_ca2;	/* 0xcc0 */
	u32 security_carveout3_ca3;	/* 0xcc4 */
	u32 security_carveout3_ca4;	/* 0xcc8 */
	u32 security_carveout3_cfia0;	/* 0xccc */
	u32 security_carveout3_cfia1;	/* 0xcd0 */
	u32 security_carveout3_cfia2;	/* 0xcd4 */
	u32 security_carveout3_cfia3;	/* 0xcd8 */
	u32 security_carveout3_cfia4;	/* 0xcdc */
	u32 rsvd_0xce0[6];			/* 0xce0-0xcf4 */

	u32 security_carveout4_cfg0;	/* 0xcf8 */
	u32 security_carveout4_bom;	/* 0xcfc */
	u32 security_carveout4_bom_hi;	/* 0xd00 */
	u32 security_carveout4_size_128kb;	/* 0xd04 */
	u32 security_carveout4_ca0;	/* 0xd08 */
	u32 security_carveout4_ca1;	/* 0xd0c */
	u32 security_carveout4_ca2;	/* 0xd10 */
	u32 security_carveout4_ca3;	/* 0xd14 */
	u32 security_carveout4_ca4;	/* 0xd18 */
	u32 security_carveout4_cfia0;	/* 0xd1c */
	u32 security_carveout4_cfia1;	/* 0xd20 */
	u32 security_carveout4_cfia2;	/* 0xd24 */
	u32 security_carveout4_cfia3;	/* 0xd28 */
	u32 security_carveout4_cfia4;	/* 0xd2c */
	u32 rsvd_0xd30[6];			/* 0xd30-0xd44 */

	u32 security_carveout5_cfg0;	/* 0xd48 */
	u32 security_carveout5_bom;	/* 0xd4c */
	u32 security_carveout5_bom_hi;	/* 0xd50 */
	u32 security_carveout5_size_128kb;	/* 0xd54 */
	u32 security_carveout5_ca0;	/* 0xd58 */
	u32 security_carveout5_ca1;	/* 0xd5c */
	u32 security_carveout5_ca2;	/* 0xd60 */
	u32 security_carveout5_ca3;	/* 0xd64 */
	u32 security_carveout5_ca4;	/* 0xd68 */
	u32 security_carveout5_cfia0;	/* 0xd6c */
	u32 security_carveout5_cfia1;	/* 0xd70 */
	u32 security_carveout5_cfia2;	/* 0xd74 */
	u32 security_carveout5_cfia3;	/* 0xd78 */
	u32 security_carveout5_cfia4;	/* 0xd7c */
};

enum {
	MC_EMEM_CFG_SIZE_MB_SHIFT = 0,
	MC_EMEM_CFG_SIZE_MB_MASK = 0x3fff,

	MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_SHIFT = 27,
	MC_EMEM_ARB_MISC0_MC_EMC_SAME_FREQ_MASK = 1 << 27,

	MC_EMEM_CFG_ACCESS_CTRL_WRITE_ACCESS_DISABLED = 1,

	MC_TIMING_CONTROL_TIMING_UPDATE = 1,
};

// 512MB Carveout size
#define MEMORY_CARVEOUT_SIZE 0x20000000

#endif