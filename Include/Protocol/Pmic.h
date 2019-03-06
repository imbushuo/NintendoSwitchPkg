#ifndef __PMIC_MANAGEMENT_PROTOCOL_H__
#define __PMIC_MANAGEMENT_PROTOCOL_H__

#include <Library/Max7762xLib.h>

#define PMIC_PROTOCOL_GUID \
    { 0x9c11c45d, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x9a, 0xd1 } }

typedef struct _PMIC_PROTOCOL PMIC_PROTOCOL;

typedef BOOLEAN (EFIAPI *power_btn_check_t)(VOID);
typedef EFI_STATUS (EFIAPI *regulator_set_power_mv_t)(UINT32 DeviceId, UINT32 Voltage);
typedef VOID (EFIAPI* regulator_enable_t)(UINT32 DeviceId);
typedef VOID (EFIAPI* regulator_disable_t)(UINT32 DeviceId);

struct _PMIC_PROTOCOL {
    power_btn_check_t           QueryPowerButton;
    regulator_set_power_mv_t    SetRegulatorVoltage;
    regulator_enable_t          EnableRegulator;
    regulator_disable_t         DisableRegulator;
};

extern EFI_GUID gPmicProtocolGuid;

#endif
