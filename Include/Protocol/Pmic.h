#ifndef __PMIC_MANAGEMENT_PROTOCOL_H__
#define __PMIC_MANAGEMENT_PROTOCOL_H__

#define PMIC_PROTOCOL_GUID \
    { 0x9c11c45d, 0xc497, 0x4e95, { 0xac, 0x18, 0x9f, 0x91, 0xca, 0x8b, 0x9a, 0xd1 } }

typedef struct _PMIC_PROTOCOL PMIC_PROTOCOL;

typedef BOOLEAN (EFIAPI *power_btn_check_t)(VOID);
typedef EFI_STATUS (EFIAPI *regulator_set_power_mv_t)(UINT32 DeviceID, UINT32 Voltage);

struct _PMIC_PROTOCOL {
    power_btn_check_t           QueryPowerButton;
    regulator_set_power_mv_t    SetRegulatorVoltage;
};

extern EFI_GUID gPmicProtocolGuid;

#endif
