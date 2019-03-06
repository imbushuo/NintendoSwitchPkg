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

TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL* mClkProtocol;
PMIC_PROTOCOL* mPmicProtocol;

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