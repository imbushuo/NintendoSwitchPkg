#ifndef __TEGRA_USB_H__
#define __TEGRA_USB_H__

#include <Protocol/Utc/Usb.h>
#include <Protocol/Utc/TegraUsb.h>

#define USB1_ADDR_MASK	0xFFFF0000

#define HOSTPC1_DEVLC	0x84
#define HOSTPC1_PSPD(x)		(((x) >> 25) & 0x3)

/* Parameters we need for USB */
enum {
	PARAM_DIVN,                     /* PLL FEEDBACK DIVIDer */
	PARAM_DIVM,                     /* PLL INPUT DIVIDER */
	PARAM_DIVP,                     /* POST DIVIDER (2^N) */
	PARAM_CPCON,                    /* BASE PLLC CHARGE Pump setup ctrl */
	PARAM_LFCON,                    /* BASE PLLC LOOP FILter setup ctrl */
	PARAM_ENABLE_DELAY_COUNT,       /* PLL-U Enable Delay Count */
	PARAM_STABLE_COUNT,             /* PLL-U STABLE count */
	PARAM_ACTIVE_DELAY_COUNT,       /* PLL-U Active delay count */
	PARAM_XTAL_FREQ_COUNT,          /* PLL-U XTAL frequency count */
	PARAM_DEBOUNCE_A_TIME,          /* 10MS DELAY for BIAS_DEBOUNCE_A */
	PARAM_BIAS_TIME,                /* 20US DELAY AFter bias cell op */

	PARAM_COUNT
};

/* Possible port types (dual role mode) */
enum dr_mode {
	DR_MODE_NONE = 0,
	DR_MODE_HOST,		/* supports host operation */
	DR_MODE_DEVICE,		/* supports device operation */
	DR_MODE_OTG,		/* supports both */
};

enum usb_ctlr_type {
	USB_CTLR_T20,
	USB_CTLR_T30,
	USB_CTLR_T114,
	USB_CTLR_T210,

	USB_CTRL_COUNT,
};

/* Information about a USB port */
struct fdt_usb {
	struct usb_ctlr *reg;	            /* address of registers in physical memory */
    unsigned utmi:1;	                /* 1 if port has external tranceiver, else 0 */
	unsigned ulpi:1;	                /* 1 if port has external ULPI transceiver */
	unsigned enabled:1;	                /* 1 to enable, 0 to disable */
	unsigned has_legacy_mode:1;         /* 1 if this port has legacy mode */
	enum usb_ctlr_type type;
	enum usb_init_type init_type;
	enum dr_mode dr_mode;	            /* dual role mode */
	enum periph_id periph_id;           /* peripheral id */
	bool vbus_gpio_valid;
	bool clk_done;
};

typedef struct fdt_usb FDT_USB, *PFDT_USB;

/*
 * This table has USB timing parameters for each Oscillator frequency we
 * support. There are four sets of values:
 *
 * 1. PLLU configuration information (reference clock is osc/clk_m and
 * PLLU-FOs are fixed at 12MHz/60MHz/480MHz).
 *
 *  Reference frequency     13.0MHz      19.2MHz      12.0MHz      26.0MHz
 *  ----------------------------------------------------------------------
 *      DIVN                960 (0x3c0)  200 (0c8)    960 (3c0h)   960 (3c0)
 *      DIVM                13 (0d)      4 (04)       12 (0c)      26 (1a)
 * Filter frequency (MHz)   1            4.8          6            2
 * CPCON                    1100b        0011b        1100b        1100b
 * LFCON0                   0            0            0            0
 *
 * 2. PLL CONFIGURATION & PARAMETERS for different clock generators:
 *
 * Reference frequency     13.0MHz         19.2MHz         12.0MHz     26.0MHz
 * ---------------------------------------------------------------------------
 * PLLU_ENABLE_DLY_COUNT   02 (0x02)       03 (03)         02 (02)     04 (04)
 * PLLU_STABLE_COUNT       51 (33)         75 (4B)         47 (2F)    102 (66)
 * PLL_ACTIVE_DLY_COUNT    05 (05)         06 (06)         04 (04)     09 (09)
 * XTAL_FREQ_COUNT        127 (7F)        187 (BB)        118 (76)    254 (FE)
 *
 * 3. Debounce values IdDig, Avalid, Bvalid, VbusValid, VbusWakeUp, and
 * SessEnd. Each of these signals have their own debouncer and for each of
 * those one out of two debouncing times can be chosen (BIAS_DEBOUNCE_A or
 * BIAS_DEBOUNCE_B).
 *
 * The values of DEBOUNCE_A and DEBOUNCE_B are calculated as follows:
 *    0xffff -> No debouncing at all
 *    <n> ms = <n> *1000 / (1/19.2MHz) / 4
 *
 * So to program a 1 ms debounce for BIAS_DEBOUNCE_A, we have:
 * BIAS_DEBOUNCE_A[15:0] = 1000 * 19.2 / 4  = 4800 = 0x12c0
 *
 * We need to use only DebounceA for BOOTROM. We don't need the DebounceB
 * values, so we can keep those to default.
 *
 * 4. The 20 microsecond delay after bias cell operation.
 */
static const unsigned T20_usb_pll[CLOCK_OSC_FREQ_COUNT][PARAM_COUNT] = {
	/* DivN, DivM, DivP, CPCON, LFCON, Delays             Debounce, Bias */
	{ 0x3C0, 0x0D, 0x00, 0xC,   0,  0x02, 0x33, 0x05, 0x7F, 0x7EF4, 5 },
	{ 0x0C8, 0x04, 0x00, 0x3,   0,  0x03, 0x4B, 0x06, 0xBB, 0xBB80, 7 },
	{ 0x3C0, 0x0C, 0x00, 0xC,   0,  0x02, 0x2F, 0x04, 0x76, 0x7530, 5 },
	{ 0x3C0, 0x1A, 0x00, 0xC,   0,  0x04, 0x66, 0x09, 0xFE, 0xFDE8, 9 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 }
};

static const unsigned T30_usb_pll[CLOCK_OSC_FREQ_COUNT][PARAM_COUNT] = {
	/* DivN, DivM, DivP, CPCON, LFCON, Delays             Debounce, Bias */
	{ 0x3C0, 0x0D, 0x00, 0xC,   1,  0x02, 0x33, 0x09, 0x7F, 0x7EF4, 5 },
	{ 0x0C8, 0x04, 0x00, 0x3,   0,  0x03, 0x4B, 0x0C, 0xBB, 0xBB80, 7 },
	{ 0x3C0, 0x0C, 0x00, 0xC,   1,  0x02, 0x2F, 0x08, 0x76, 0x7530, 5 },
	{ 0x3C0, 0x1A, 0x00, 0xC,   1,  0x04, 0x66, 0x09, 0xFE, 0xFDE8, 9 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 }
};

static const unsigned T114_usb_pll[CLOCK_OSC_FREQ_COUNT][PARAM_COUNT] = {
	/* DivN, DivM, DivP, CPCON, LFCON, Delays             Debounce, Bias */
	{ 0x3C0, 0x0D, 0x00, 0xC,   2,  0x02, 0x33, 0x09, 0x7F, 0x7EF4, 6 },
	{ 0x0C8, 0x04, 0x00, 0x3,   2,  0x03, 0x4B, 0x0C, 0xBB, 0xBB80, 8 },
	{ 0x3C0, 0x0C, 0x00, 0xC,   2,  0x02, 0x2F, 0x08, 0x76, 0x7530, 5 },
	{ 0x3C0, 0x1A, 0x00, 0xC,   2,  0x04, 0x66, 0x09, 0xFE, 0xFDE8, 11 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 },
	{ 0x000, 0x00, 0x00, 0x0,   0,  0x00, 0x00, 0x00, 0x00, 0x0000, 0 }
};

/* NOTE: 13/26MHz settings are N/A for T210, so dupe 12MHz settings for now */
static const unsigned T210_usb_pll[CLOCK_OSC_FREQ_COUNT][PARAM_COUNT] = {
	/* DivN, DivM, DivP, KCP,   KVCO,  Delays              Debounce, Bias */
	{ 0x028, 0x01, 0x01, 0x0,   0,  0x02, 0x2F, 0x08, 0x76,  32500,  5 },
	{ 0x019, 0x01, 0x01, 0x0,   0,  0x03, 0x4B, 0x0C, 0xBB,  48000,  8 },
	{ 0x028, 0x01, 0x01, 0x0,   0,  0x02, 0x2F, 0x08, 0x76,  30000,  5 },
	{ 0x028, 0x01, 0x01, 0x0,   0,  0x02, 0x2F, 0x08, 0x76,  65000,  5 },
	{ 0x019, 0x02, 0x01, 0x0,   0,  0x05, 0x96, 0x18, 0x177, 96000, 15 },
	{ 0x028, 0x04, 0x01, 0x0,   0,  0x04, 0x66, 0x09, 0xFE, 120000, 20 }
};

/* UTMIP Idle Wait Delay */
static const u8 utmip_idle_wait_delay = 17;

/* UTMIP Elastic limit */
static const u8 utmip_elastic_limit = 16;

/* UTMIP High Speed Sync Start Delay */
static const u8 utmip_hs_sync_start_delay = 9;

struct fdt_usb_controller {
	/* flag to determine whether controller supports hostpc register */
	u32 has_hostpc:1;
	const unsigned *pll_parameter;
};

static struct fdt_usb_controller fdt_usb_controllers[USB_CTRL_COUNT] = {
	{
		.has_hostpc	= 0,
		.pll_parameter	= (const unsigned *)T20_usb_pll,
	},
	{
		.has_hostpc	= 1,
		.pll_parameter	= (const unsigned *)T30_usb_pll,
	},
	{
		.has_hostpc	= 1,
		.pll_parameter	= (const unsigned *)T114_usb_pll,
	},
	{
		.has_hostpc	= 1,
		.pll_parameter	= (const unsigned *)T210_usb_pll,
	},
};

#endif