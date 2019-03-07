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

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Library/I2cLib.h>
#include <Device/Max77620.h>
#include <Device/Pmc.h>
#include <Library/ClockLib.h>
#include <Library/Max7762xLib.h>
#include <Library/EarlyTimerLib.h>
#include <Protocol/ClockManagement.h>
#include <Protocol/Pmic.h>

TEGRA210_CLOCK_MGMT_PROTOCOL *mClockProtocol;

STATIC
EFIAPI
BOOLEAN
QueryPowerButton
(
    VOID
)
{
    return (max77620_recv_byte(MAX77620_REG_ONOFFSTAT) & 0x4);
}

STATIC
EFIAPI
EFI_STATUS
SetRegulatorVoltage
(
    UINT32 DeviceId,
    UINT32 Voltage
)
{
    int ret = 0;
    ret = max77620_regulator_set_voltage(DeviceId, Voltage);

    if (ret != 1) return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

STATIC
EFIAPI
VOID
EnableRegulator
(
    UINT32 DeviceId
)
{
    max77620_regulator_enable(DeviceId, 1);
}

STATIC
EFIAPI
VOID
DisableRegulator
(
    UINT32 DeviceId
)
{
    max77620_regulator_enable(DeviceId, 0);
}


STATIC PMIC_PROTOCOL mPmicProto = {
    QueryPowerButton,
    SetRegulatorVoltage,
    EnableRegulator,
    DisableRegulator
};

EFI_STATUS
EFIAPI
PmicDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE ProtoHandle = NULL;
    UINT8 bVal;

    Status = gBS->LocateProtocol(
        &gTegra210ClockManagementProtocolGuid,
        NULL,
        (VOID**) &mClockProtocol
    );
    if (EFI_ERROR(Status)) goto exit;

    // Clock init
    Status = mClockProtocol->EnableI2c(I2C_1);
    if (EFI_ERROR(Status)) goto exit;

    Status = mClockProtocol->EnableI2c(I2C_5);
    if (EFI_ERROR(Status)) goto exit;

    // I2C init
    i2c_init(I2C_1);
    i2c_init(I2C_5);

	// Start up the SDMMC1 IO voltage regulator @ 3.3V
    max77620_send_byte(MAX77620_REG_LDO2_CFG, 0xF2);

    // Disable LDO4 discharge
    bVal = max77620_recv_byte(MAX77620_REG_LDO4_CFG);
    bVal &= ~BIT(1);
    max77620_send_byte(MAX77620_REG_LDO4_CFG, bVal);

    // Set MBLPD
    bVal = max77620_recv_byte(MAX77620_REG_CNFGGLBL1);
    bVal |= BIT(6);
    max77620_send_byte(MAX77620_REG_CNFGGLBL1, bVal);

    // Start up the PCIe power
    max77620_send_byte(MAX77620_REG_LDO1_CFG, 0xCA);

    // Install protocol
    Status = gBS->InstallMultipleProtocolInterfaces(
        &ProtoHandle,
        &gPmicProtocolGuid,
        &mPmicProto,
        NULL
    );

exit:
    ASSERT_EFI_ERROR(Status);
    return Status;
}
