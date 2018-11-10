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
};

// 512MB Carveout size
#define MEMORY_CARVEOUT_SIZE 0x20000000

#endif