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
#include <Library/ClockLib.h>
#include <Protocol/ClockManagement.h>

STATIC
EFI_STATUS
EFIAPI
EnableI2c
(
    IN UINT32 DeviceIndex
);

STATIC
EFI_STATUS
EFIAPI
EnableUart
(
    IN UINT32 DeviceIndex
);

STATIC TEGRA210_CLOCK_MGMT_PROTOCOL mClockProto = {
    EnableUart,
    EnableI2c,
};

STATIC
EFI_STATUS
EFIAPI
EnableI2c
(
    IN UINT32 DeviceIndex
)
{
    if (DeviceIndex < 0 || DeviceIndex > 5) return EFI_INVALID_PARAMETER;
    clock_enable_i2c(DeviceIndex);
    return EFI_SUCCESS; 
}

STATIC
EFI_STATUS
EFIAPI
EnableUart
(
    IN UINT32 DeviceIndex
)
{
    if (DeviceIndex < 0 || DeviceIndex > 4) return EFI_INVALID_PARAMETER;
    clock_enable_uart(DeviceIndex);
    return EFI_SUCCESS; 
}

EFI_STATUS
EFIAPI
ClockManagementDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE ProtoHandle = NULL;

    Status = gBS->InstallMultipleProtocolInterfaces(
        &ProtoHandle,
        &gTegra210ClockManagementProtocolGuid,
        &mClockProto,
        NULL
    );

    ASSERT_EFI_ERROR(Status);
    return Status;
}
