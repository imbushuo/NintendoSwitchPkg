/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  Copyright (c) 2016, Linaro, Ltd. All rights reserved.<BR>
  Copyright (c) 2019, Bingxing Wang. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/UBootClockManagement.h>
#include <Protocol/Utc/Clock.h>
#include <Protocol/Utc/ClkRst.h>
#include <Protocol/Utc/ErrNo.h>
#include <Protocol/Utc/Usb.h>
#include <Protocol/Utc/Ehci.h>
#include <Protocol/Utc/Tegra.h>

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Device/Pmc.h>
#include <Library/GpioLib.h>
#include <Protocol/Pmic.h>
#include <Shim/DebugLib.h>
#include <Shim/UBootIo.h>
#include <Shim/TimerLib.h>
#include <Shim/BitOps.h>

#include "TegraUsb.h"

#define HOST_CONTROLLER_OPERATION_REG_SIZE  0x44
TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL* mClkProtocol;
FDT_USB mPriv;

static void usbf_reset_controller(
    struct fdt_usb *config,
    struct usb_ctlr *usbctlr
)
{
	/* Reset the USB controller with 2us delay */
	mClkProtocol->ResetPeriph(config->periph_id, 2);

	/*
	 * Set USB1_NO_LEGACY_MODE to 1, Registers are accessible under
	 * base address
	 */
	if (config->has_legacy_mode)
	{
        setbits_le32(&usbctlr->usb1_legacy_ctrl, USB1_NO_LEGACY_MODE);
    }

	/* Put UTMIP1/3 in reset */
	setbits_le32(&usbctlr->susp_ctrl, UTMIP_RESET);

	/* Enable the UTMIP PHY */
	if (config->utmi)
	{
        setbits_le32(&usbctlr->susp_ctrl, UTMIP_PHY_ENB);
    }
}

static const unsigned *get_pll_timing(struct fdt_usb_controller *controller)
{
	const unsigned *timing;
	timing = controller->pll_parameter + mClkProtocol->GetOscFreq() * PARAM_COUNT;

	return timing;
}

/* select the PHY to use with a USB controller */
static void init_phy_mux(struct fdt_usb *config, uint pts,
			 enum usb_init_type init)
{
	struct usb_ctlr *usbctlr = config->reg;

#if defined(CONFIG_TEGRA20)
	if (config->periph_id == PERIPH_ID_USBD) {
		clrsetbits_le32(&usbctlr->port_sc1, PTS1_MASK,
				pts << PTS1_SHIFT);
		clrbits_le32(&usbctlr->port_sc1, STS1);
	} else {
		clrsetbits_le32(&usbctlr->port_sc1, PTS_MASK,
				pts << PTS_SHIFT);
		clrbits_le32(&usbctlr->port_sc1, STS);
	}
#else
	/* Set to Host mode (if applicable) after Controller Reset was done */
	clrsetbits_le32(&usbctlr->usb_mode, USBMODE_CM_HC,
			(init == USB_INIT_HOST) ? USBMODE_CM_HC : 0);
	/*
	 * Select PHY interface after setting host mode.
	 * For device mode, the ordering requirement is not an issue, since
	 * only the first USB controller supports device mode, and that USB
	 * controller can only talk to a UTMI PHY, so the PHY selection is
	 * already made at reset time, so this write is a no-op.
	 */
	clrsetbits_le32(&usbctlr->hostpc1_devlc, PTS_MASK,
			pts << PTS_SHIFT);
	clrbits_le32(&usbctlr->hostpc1_devlc, STS);
#endif
}

/* set up the UTMI USB controller with the parameters provided */
static int init_utmi_usb_controller(
    struct fdt_usb *config,
    enum usb_init_type init
)
{
	struct fdt_usb_controller *controller;
	u32 b_sess_valid_mask, val;
	int loop_count;
	const unsigned *timing;
	struct usb_ctlr *usbctlr = config->reg;
	struct clk_rst_ctlr *clkrst;
	struct usb_ctlr *usb1ctlr;

    mClkProtocol->EnableClk(config->periph_id);

	/* Reset the usb controller */
	usbf_reset_controller(config, usbctlr);

	/* Stop crystal clock by setting UTMIP_PHY_XTAL_CLOCKEN low */
	clrbits_le32(&usbctlr->utmip_misc_cfg1, UTMIP_PHY_XTAL_CLOCKEN);

	/* Follow the crystal clock disable by >100ns delay */
	udelay(1);

	b_sess_valid_mask = (VBUS_B_SESS_VLD_SW_VALUE | VBUS_B_SESS_VLD_SW_EN);
	clrsetbits_le32(&usbctlr->phy_vbus_sensors, b_sess_valid_mask,
			(init == USB_INIT_DEVICE) ? b_sess_valid_mask : 0);

	/*
	 * To Use the A Session Valid for cable detection logic, VBUS_WAKEUP
	 * mux must be switched to actually use a_sess_vld threshold.
	 */
	if (config->dr_mode == DR_MODE_OTG && config->vbus_gpio_valid)
	{
        clrsetbits_le32(
            &usbctlr->usb1_legacy_ctrl,
			VBUS_SENSE_CTL_MASK,
			VBUS_SENSE_CTL_A_SESS_VLD << VBUS_SENSE_CTL_SHIFT
        );
    }

	controller = &fdt_usb_controllers[config->type];
	debug("controller=%p, type=%d\n", controller, config->type);

	/*
	 * PLL Delay CONFIGURATION settings. The following parameters control
	 * the bring up of the plls.
	 */
	timing = get_pll_timing(controller);

	if (!controller->has_hostpc) {
		val = readl(&usbctlr->utmip_misc_cfg1);
		clrsetbits_le32(&val, UTMIP_PLLU_STABLE_COUNT_MASK,
				timing[PARAM_STABLE_COUNT] <<
				UTMIP_PLLU_STABLE_COUNT_SHIFT);
		clrsetbits_le32(&val, UTMIP_PLL_ACTIVE_DLY_COUNT_MASK,
				timing[PARAM_ACTIVE_DELAY_COUNT] <<
				UTMIP_PLL_ACTIVE_DLY_COUNT_SHIFT);
		writel(val, &usbctlr->utmip_misc_cfg1);

		/* Set PLL enable delay count and crystal frequency count */
		val = readl(&usbctlr->utmip_pll_cfg1);
		clrsetbits_le32(&val, UTMIP_PLLU_ENABLE_DLY_COUNT_MASK,
				timing[PARAM_ENABLE_DELAY_COUNT] <<
				UTMIP_PLLU_ENABLE_DLY_COUNT_SHIFT);
		clrsetbits_le32(&val, UTMIP_XTAL_FREQ_COUNT_MASK,
				timing[PARAM_XTAL_FREQ_COUNT] <<
				UTMIP_XTAL_FREQ_COUNT_SHIFT);
		writel(val, &usbctlr->utmip_pll_cfg1);
	} else {
		clkrst = (struct clk_rst_ctlr *) NV_PA_CLK_RST_BASE;

		val = readl(&clkrst->crc_utmip_pll_cfg2);
		clrsetbits_le32(&val, UTMIP_PLLU_STABLE_COUNT_MASK,
				timing[PARAM_STABLE_COUNT] <<
				UTMIP_PLLU_STABLE_COUNT_SHIFT);
		clrsetbits_le32(&val, UTMIP_PLL_ACTIVE_DLY_COUNT_MASK,
				timing[PARAM_ACTIVE_DELAY_COUNT] <<
				UTMIP_PLL_ACTIVE_DLY_COUNT_SHIFT);
		writel(val, &clkrst->crc_utmip_pll_cfg2);

		/* Set PLL enable delay count and crystal frequency count */
		val = readl(&clkrst->crc_utmip_pll_cfg1);
		clrsetbits_le32(&val, UTMIP_PLLU_ENABLE_DLY_COUNT_MASK,
				timing[PARAM_ENABLE_DELAY_COUNT] <<
				UTMIP_PLLU_ENABLE_DLY_COUNT_SHIFT);
		clrsetbits_le32(&val, UTMIP_XTAL_FREQ_COUNT_MASK,
				timing[PARAM_XTAL_FREQ_COUNT] <<
				UTMIP_XTAL_FREQ_COUNT_SHIFT);
		writel(val, &clkrst->crc_utmip_pll_cfg1);

		/* Disable Power Down state for PLL */
		clrbits_le32(&clkrst->crc_utmip_pll_cfg1,
			     PLLU_POWERDOWN | PLL_ENABLE_POWERDOWN |
			     PLL_ACTIVE_POWERDOWN);

		/* Recommended PHY settings for EYE diagram */
		val = readl(&usbctlr->utmip_xcvr_cfg0);
		clrsetbits_le32(&val, UTMIP_XCVR_SETUP_MASK,
				0x4 << UTMIP_XCVR_SETUP_SHIFT);
		clrsetbits_le32(&val, UTMIP_XCVR_SETUP_MSB_MASK,
				0x3 << UTMIP_XCVR_SETUP_MSB_SHIFT);
		clrsetbits_le32(&val, UTMIP_XCVR_HSSLEW_MSB_MASK,
				0x8 << UTMIP_XCVR_HSSLEW_MSB_SHIFT);
		writel(val, &usbctlr->utmip_xcvr_cfg0);
		clrsetbits_le32(&usbctlr->utmip_xcvr_cfg1,
				UTMIP_XCVR_TERM_RANGE_ADJ_MASK,
				0x7 << UTMIP_XCVR_TERM_RANGE_ADJ_SHIFT);

		/* Some registers can be controlled from USB1 only. */
		if (config->periph_id != PERIPH_ID_USBD) {
			mClkProtocol->EnableClk(PERIPH_ID_USBD);
			/* Disable Reset if in Reset state */
			mClkProtocol->DeassertRst(PERIPH_ID_USBD);
		}
		usb1ctlr = (struct usb_ctlr *)
			((unsigned long)config->reg & USB1_ADDR_MASK);
		val = readl(&usb1ctlr->utmip_bias_cfg0);
		setbits_le32(&val, UTMIP_HSDISCON_LEVEL_MSB);
		clrsetbits_le32(&val, UTMIP_HSDISCON_LEVEL_MASK,
				0x1 << UTMIP_HSDISCON_LEVEL_SHIFT);
		clrsetbits_le32(&val, UTMIP_HSSQUELCH_LEVEL_MASK,
				0x2 << UTMIP_HSSQUELCH_LEVEL_SHIFT);
		writel(val, &usb1ctlr->utmip_bias_cfg0);

		/* Miscellaneous setting mentioned in Programming Guide */
		clrbits_le32(&usbctlr->utmip_misc_cfg0,
			     UTMIP_SUSPEND_EXIT_ON_EDGE);
	}

	/* Setting the tracking length time */
	clrsetbits_le32(&usbctlr->utmip_bias_cfg1,
		UTMIP_BIAS_PDTRK_COUNT_MASK,
		timing[PARAM_BIAS_TIME] << UTMIP_BIAS_PDTRK_COUNT_SHIFT);

	/* Program debounce time for VBUS to become valid */
	clrsetbits_le32(&usbctlr->utmip_debounce_cfg0,
		UTMIP_DEBOUNCE_CFG0_MASK,
		timing[PARAM_DEBOUNCE_A_TIME] << UTMIP_DEBOUNCE_CFG0_SHIFT);

	if (timing[PARAM_DEBOUNCE_A_TIME] > 0xFFFF) {
		clrsetbits_le32(&usbctlr->utmip_debounce_cfg0,
				UTMIP_DEBOUNCE_CFG0_MASK,
				(timing[PARAM_DEBOUNCE_A_TIME] >> 1)
				<< UTMIP_DEBOUNCE_CFG0_SHIFT);
		clrsetbits_le32(&usbctlr->utmip_bias_cfg1,
				UTMIP_BIAS_DEBOUNCE_TIMESCALE_MASK,
				1 << UTMIP_BIAS_DEBOUNCE_TIMESCALE_SHIFT);
	}

	setbits_le32(&usbctlr->utmip_tx_cfg0, UTMIP_FS_PREAMBLE_J);

	/* Disable battery charge enabling bit */
	setbits_le32(&usbctlr->utmip_bat_chrg_cfg0, UTMIP_PD_CHRG);

	clrbits_le32(&usbctlr->utmip_xcvr_cfg0, UTMIP_XCVR_LSBIAS_SE);
	setbits_le32(&usbctlr->utmip_spare_cfg0, FUSE_SETUP_SEL);

	/*
	 * Configure the UTMIP_IDLE_WAIT and UTMIP_ELASTIC_LIMIT
	 * Setting these fields, together with default values of the
	 * other fields, results in programming the registers below as
	 * follows:
	 *         UTMIP_HSRX_CFG0 = 0x9168c000
	 *         UTMIP_HSRX_CFG1 = 0x13
	 */

	/* Set PLL enable delay count and Crystal frequency count */
	val = readl(&usbctlr->utmip_hsrx_cfg0);
	clrsetbits_le32(&val, UTMIP_IDLE_WAIT_MASK,
		utmip_idle_wait_delay << UTMIP_IDLE_WAIT_SHIFT);
	clrsetbits_le32(&val, UTMIP_ELASTIC_LIMIT_MASK,
		utmip_elastic_limit << UTMIP_ELASTIC_LIMIT_SHIFT);
	writel(val, &usbctlr->utmip_hsrx_cfg0);

	/* Configure the UTMIP_HS_SYNC_START_DLY */
	clrsetbits_le32(&usbctlr->utmip_hsrx_cfg1,
		UTMIP_HS_SYNC_START_DLY_MASK,
		utmip_hs_sync_start_delay << UTMIP_HS_SYNC_START_DLY_SHIFT);

	/* Preceed the crystal clock disable by >100ns delay. */
	udelay(1);

	/* Resuscitate crystal clock by setting UTMIP_PHY_XTAL_CLOCKEN */
	setbits_le32(&usbctlr->utmip_misc_cfg1, UTMIP_PHY_XTAL_CLOCKEN);

	if (controller->has_hostpc) {
		if (config->periph_id == PERIPH_ID_USBD)
			clrbits_le32(&clkrst->crc_utmip_pll_cfg2,
				     UTMIP_FORCE_PD_SAMP_A_POWERDOWN);
		if (config->periph_id == PERIPH_ID_USB2)
			clrbits_le32(&clkrst->crc_utmip_pll_cfg2,
				     UTMIP_FORCE_PD_SAMP_B_POWERDOWN);
		if (config->periph_id == PERIPH_ID_USB3)
			clrbits_le32(&clkrst->crc_utmip_pll_cfg2,
				     UTMIP_FORCE_PD_SAMP_C_POWERDOWN);
	}
	/* Finished the per-controller init. */

	/* De-assert UTMIP_RESET to bring out of reset. */
	clrbits_le32(&usbctlr->susp_ctrl, UTMIP_RESET);

	/* Wait for the phy clock to become valid in 100 ms */
	for (loop_count = 100000; loop_count != 0; loop_count--) {
		if (readl(&usbctlr->susp_ctrl) & USB_PHY_CLK_VALID)
			break;
		udelay(1);
	}
	if (!loop_count) return -ETIMEDOUT;

	/* Disable ICUSB FS/LS transceiver */
	clrbits_le32(&usbctlr->icusb_ctrl, IC_ENB1);

	/* Select UTMI parallel interface */
	init_phy_mux(config, PTS_UTMI, init);

	/* Deassert power down state */
	clrbits_le32(&usbctlr->utmip_xcvr_cfg0, UTMIP_FORCE_PD_POWERDOWN |
		UTMIP_FORCE_PD2_POWERDOWN | UTMIP_FORCE_PDZI_POWERDOWN);
	clrbits_le32(&usbctlr->utmip_xcvr_cfg1, UTMIP_FORCE_PDDISC_POWERDOWN |
		UTMIP_FORCE_PDCHRP_POWERDOWN | UTMIP_FORCE_PDDR_POWERDOWN);

	if (controller->has_hostpc) {
		/*
		 * BIAS Pad Power Down is common among all 3 USB
		 * controllers and can be controlled from USB1 only.
		 */
		usb1ctlr = (struct usb_ctlr *)
			((unsigned long)config->reg & USB1_ADDR_MASK);
		clrbits_le32(&usb1ctlr->utmip_bias_cfg0, UTMIP_BIASPD);
		udelay(25);
		clrbits_le32(&usb1ctlr->utmip_bias_cfg1,
			     UTMIP_FORCE_PDTRK_POWERDOWN);
	}
	return 0;
}

/* Set up VBUS for host/device mode */
static void set_up_vbus(
    struct fdt_usb *config, 
    enum usb_init_type init
)
{
	/*
	 * If we are an OTG port initializing in host mode,
	 * check if remote host is driving VBus and bail out in this case.
	 */
	if (init == USB_INIT_HOST &&
	    config->dr_mode == DR_MODE_OTG &&
	    (readl(&config->reg->phy_vbus_sensors) & VBUS_VLD_STS)) {
		printf("tegrausb: VBUS input active; not enabling as host\n");
		return;
	}

	if (config->vbus_gpio_valid) 
    {
		int vbus_value;
		vbus_value = (init == USB_INIT_HOST);

        // nvidia,vbus-gpio = <&gpio TEGRA_GPIO(CC, 4) GPIO_ACTIVE_HIGH>
        gpio_config(GPIO_PORT_CC, GPIO_PIN_4, GPIO_MODE_GPIO);
	    gpio_write(GPIO_PORT_CC, GPIO_PIN_4, GPIO_HIGH);
	    gpio_output_enable(GPIO_PORT_CC, GPIO_PIN_4, GPIO_OUTPUT_ENABLE);
		debug("set_up_vbus: done \n");
	}
}

int usb_common_init(struct fdt_usb *config, enum usb_init_type init)
{
	int ret = 0;

	switch (init) {
	case USB_INIT_HOST:
		switch (config->dr_mode) {
		case DR_MODE_HOST:
		case DR_MODE_OTG:
			break;
		default:
			printf("tegrausb: Invalid dr_mode %d for host mode\n", config->dr_mode);
			return -1;
		}
		break;
	case USB_INIT_DEVICE:
		if (config->periph_id != PERIPH_ID_USBD) {
			printf("tegrausb: Device mode only supported on first USB controller\n");
			return -1;
		}
		if (!config->utmi) {
			printf("tegrausb: Device mode only supported with UTMI PHY\n");
			return -1;
		}
		switch (config->dr_mode) {
		case DR_MODE_DEVICE:
		case DR_MODE_OTG:
			break;
		default:
			printf("tegrausb: Invalid dr_mode %d for device mode\n", config->dr_mode);
			return -1;
		}
		break;
	default:
		printf("tegrausb: Unknown USB_INIT_* %d\n", init);
		return -1;
	}

	debug("%d, %d\n", config->utmi, config->ulpi);
	if (config->utmi)
	{
        ret = init_utmi_usb_controller(config, init);
    }
	else if (config->ulpi)
	{
        printf("tegrausb: only UTMI is currently supported \n");
        return -1;
    }

	if (ret) return ret;

	set_up_vbus(config, init);
	config->init_type = init;

	return 0;
}

static void config_clock(const u32 timing[])
{
	debug("%s: DIVM = %d, DIVN = %d, DIVP = %d, cpcon/lfcon = %d/%d\n",
	      __func__, timing[PARAM_DIVM], timing[PARAM_DIVN],
	      timing[PARAM_DIVP], timing[PARAM_CPCON], timing[PARAM_LFCON]);

	mClkProtocol->StartPll(CLOCK_ID_USB,
		timing[PARAM_DIVM], timing[PARAM_DIVN], timing[PARAM_DIVP],
		timing[PARAM_CPCON], timing[PARAM_LFCON]);
}

EFI_STATUS
ConfigController(
    VOID
)
{
    EFI_STATUS Status;
    UINT8      Data = 0;
    int ret;

    mPriv.reg = (VOID*) (UINTN) 0x7d000000;
    mPriv.enabled = 1;
    mPriv.utmi = 1;
    mPriv.ulpi = 0;
    mPriv.has_legacy_mode = 0;
    mPriv.dr_mode = DR_MODE_OTG;
    mPriv.type = USB_CTLR_T210;
    mPriv.init_type = USB_INIT_HOST;
    mPriv.periph_id = PERIPH_ID_USBD;
    mPriv.vbus_gpio_valid = true;
    mPriv.clk_done = false;

    ret = usb_common_init(&mPriv, USB_INIT_HOST);
    if (ret) return EFI_DEVICE_ERROR;

    if (!mPriv.clk_done)
    {
        config_clock(get_pll_timing(&fdt_usb_controllers[mPriv.type]));
        mPriv.clk_done = true;
    }

    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ConfigureUSBHost(
    NON_DISCOVERABLE_DEVICE   *Device
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PciEmulationEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;

    Status = gBS->LocateProtocol(
        &gTegraUBootClockManagementProtocolGuid,
        NULL,
        (VOID**) &mClkProtocol
    );

    if (EFI_ERROR(Status)) goto exit;

    Status = ConfigController();
    if (EFI_ERROR(Status)) goto exit;
    return RegisterNonDiscoverableMmioDevice(
        NonDiscoverableDeviceTypeEhci,
        NonDiscoverableDeviceDmaTypeNonCoherent,
        ConfigureUSBHost,
        NULL,
        1,
        &mPriv.reg->cap_length, 
        0x740
    );

exit:
    ASSERT_EFI_ERROR(Status);
    return Status;
}
