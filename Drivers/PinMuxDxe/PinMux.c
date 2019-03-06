#include <PiDxe.h>
#include <Uefi.h>
#include <Protocol/Utc/Tegra210/PinMux.h>
#include <Protocol/Utc/Tegra210/Sysctl.h>
#include <Protocol/Utc/ErrNo.h>
#include <Library/IoLib.h>
#include <Foundation/Types.h>
#include <Shim/DebugLib.h>
#include <Shim/TimerLib.h>
#include <Shim/UBootIo.h>
#include <Library/TimerLib.h>
#include <Device/PinMux.h>
#include <Protocol/PinMux.h>

#include "Include/Gpio.h"

TEGRA_PINMUX_PROTOCOL mPinMuxProtocol = {
    pinmux_set_tristate_input_clamping,
    pinmux_clear_tristate_input_clamping,
    pinmux_set_func,
    pinmux_set_pullupdown,
    pinmux_tristate_enable,
    pinmux_tristate_disable
};

EFI_STATUS
EFIAPI
PinMuxDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE ProtoHandle = NULL;

    pinmux_clear_tristate_input_clamping();

    gpio_config_table(nintendo_switch_gpio_inits,
        ARRAY_SIZE(nintendo_switch_gpio_inits));

	pinmux_config_pingrp_table(nintendo_switch_pingrps,
        ARRAY_SIZE(nintendo_switch_pingrps));

	pinmux_config_drvgrp_table(nintendo_switch_drvgrps,
        ARRAY_SIZE(nintendo_switch_drvgrps));

    Status = gBS->InstallMultipleProtocolInterfaces(
        &ProtoHandle,
        &gTegraPinMuxProtocolGuid,
        mPinMuxProtocol,
        NULL
    );

    ASSERT_EFI_ERROR(Status);
    return Status;
}