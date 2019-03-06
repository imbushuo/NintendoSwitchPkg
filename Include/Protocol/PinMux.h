#ifndef __PINMUX_MGMT_PROTOCOL_H__
#define __PINMUX_MGMT_PROTOCOL_H__

#include <PiDxe.h>
#include <Uefi.h>
#include <Foundation/Types.h>
#include <Protocol/Utc/Tegra210/PinMux.h>
#include <Protocol/Utc/Tegra210/Sysctl.h>

#define TEGRA_PINMUX_PROTOCOL_GUID \
    { 0x9c11c45d, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x15, 0xd1 } }

typedef struct _TEGRA_PINMUX_PROTOCOL TEGRA_PINMUX_PROTOCOL;

typedef VOID (EFIAPI* set_tristate_input_clamping_t)(VOID);
typedef VOID (EFIAPI* clear_tristate_input_clamping_t)(VOID);
typedef VOID (EFIAPI* set_func_t)(enum pmux_pingrp pin, enum pmux_func func);
typedef VOID (EFIAPI* set_pullupdown_t)(enum pmux_pingrp pin, enum pmux_pull pupd);
typedef VOID (EFIAPI* tristate_enable_t)(enum pmux_pingrp pin);
typedef VOID (EFIAPI* tristate_disable_t)(enum pmux_pingrp pin);

struct _TEGRA_PINMUX_PROTOCOL {
    set_tristate_input_clamping_t SetTristateInputClamping;
    clear_tristate_input_clamping_t ClearTristateInputClamping;
    set_func_t SetFunction;
    set_pullupdown_t SetPullUpDown;
    tristate_enable_t EnableTriState;
    tristate_disable_t DisableTriState;
};

extern EFI_GUID gTegraPinMuxProtocolGuid;

#endif