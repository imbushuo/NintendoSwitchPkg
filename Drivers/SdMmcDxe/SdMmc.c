#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/BaseLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DevicePathLib.h>

#include "Library/sdmmc.h"

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Library/Max7762xLib.h>
#include <Library/GpioLib.h>
#include <Library/PinmuxLib.h>

sdmmc_t mController;
sdmmc_storage_t mStorage;

STATIC
VOID
InitializePinmux(
    VOID
)
{
    // Configure SD card detect pin
	pinmux_set_config(PINMUX_GPIO_Z1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP | PINMUX_GPIO_PZ1_FUNC_SDMMC1);
	gpio_config(GPIO_DECOMPOSE(GPIO_Z1_INDEX), GPIO_MODE_GPIO);
	gpio_output_enable(GPIO_DECOMPOSE(GPIO_Z1_INDEX), GPIO_OUTPUT_DISABLE);
	APB_MISC(APB_MISC_GP_VGPIO_GPIO_MUX_SEL) = 0; // Use GPIO for all SDMMC 

	// Configure SD power enable pin (powered off by default)
	pinmux_set_config(PINMUX_DMIC3_CLK_INDEX, PINMUX_INPUT_ENABLE | PINMUX_PULL_DOWN | PINMUX_DMIC3_CLK_FUNC_I2S5A); //not sure about the altfunc here
	gpio_config(GPIO_BY_NAME(DMIC3_CLK), GPIO_MODE_GPIO);
	gpio_write(GPIO_BY_NAME(DMIC3_CLK), GPIO_LOW);
	gpio_output_enable(GPIO_BY_NAME(DMIC3_CLK), GPIO_OUTPUT_ENABLE);
}

EFI_STATUS
EFIAPI
SdMmcDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    int ret = 0;

    InitializePinmux();
    ret = sdmmc_storage_init_sd(&mStorage, &mController, SDMMC_1, SDMMC_BUS_WIDTH_4, 11);
    
    return EFI_SUCCESS;
}