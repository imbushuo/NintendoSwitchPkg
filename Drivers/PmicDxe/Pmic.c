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

STATIC PMIC_PROTOCOL mPmicProto = {
    QueryPowerButton,
    SetRegulatorVoltage,
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

    Status = gBS->LocateProtocol(
        &gTegra210ClockManagementProtocolGuid,
        NULL,
        (VOID**) &mClockProtocol
    );
    if (EFI_ERROR(Status)) goto exit;

    // Clock init
    Status = mClockProtocol->EnableClDvfs();
    if (EFI_ERROR(Status)) goto exit;

    Status = mClockProtocol->EnableI2c(I2C_1);
    if (EFI_ERROR(Status)) goto exit;

    Status = mClockProtocol->EnableI2c(I2C_5);
    if (EFI_ERROR(Status)) goto exit;

    // I2C init
    i2c_init(I2C_1);
    i2c_init(I2C_5);

    // PMIC, SDMMC1 power init
    max77620_send_byte(MAX77620_REG_CNFGBBC, 0x40);
	max77620_send_byte(MAX77620_REG_ONOFFCNFG1, 0x78);
    max77620_send_byte(MAX77620_REG_FPS_CFG0, 0x38);
	max77620_send_byte(MAX77620_REG_FPS_CFG1, 0x3A);
	max77620_send_byte(MAX77620_REG_FPS_CFG2, 0x38);
	max77620_regulator_config_fps(REGULATOR_LDO4);
	max77620_regulator_config_fps(REGULATOR_LDO8);
	max77620_regulator_config_fps(REGULATOR_SD0);
	max77620_regulator_config_fps(REGULATOR_SD1);
	max77620_regulator_config_fps(REGULATOR_SD3);
    max77620_regulator_set_voltage(REGULATOR_SD0, 1125000); // 1.125V

    // Set SDMMC1 IO clamps to default value before changing voltage
	PMC(APBDEV_PMC_PWR_DET_VAL) |= (1 << 12);

	// Start up the SDMMC1 IO voltage regulator
	max77620_regulator_set_voltage(REGULATOR_LDO2, 3300000);
	max77620_regulator_enable(REGULATOR_LDO2, 1);

	// Remove isolation from SDMMC1 and core domain
	PMC(APBDEV_PMC_NO_IOPOWER) &= ~(1 << 12);

    CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) = (CLOCK(CLK_RST_CONTROLLER_SCLK_BURST_POLICY) & 0xFFFF8888) | 0x3333;

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
