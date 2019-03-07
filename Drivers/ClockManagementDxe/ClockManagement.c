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
#include <Protocol/UBootClockManagement.h>
#include "Include/UBootClkImpl.h"

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

STATIC
EFI_STATUS
EFIAPI
EnableClDvfs
(
    VOID
);

STATIC
BOOLEAN
EFIAPI
SdMmcClockNotResetAndEnabled
(
    UINT32 Index
);

STATIC
EFI_STATUS
EFIAPI
SdMmcClockGetParams
(
    UINT32  *pOut,
    UINT16  *pDivisor,
    UINT32  Type
);

STATIC
EFI_STATUS
EFIAPI
SdMmcClockEnable
(
    UINT32  Index,
    UINT32  Clock
);

STATIC
EFI_STATUS
EFIAPI
SdMmcClockDisable
(
    UINT32 Index
);

STATIC
EFI_STATUS
EFIAPI
SdMmcClockConfigSource
(
    UINT32 *pOut,
    UINT32 DeviceId,
    UINT32 Value
);

STATIC TEGRA210_CLOCK_MGMT_PROTOCOL mClockProto = {
    EnableUart,
    EnableI2c,
    EnableClDvfs,
    SdMmcClockNotResetAndEnabled,
    SdMmcClockGetParams,
    SdMmcClockEnable,
    SdMmcClockConfigSource,
    SdMmcClockDisable
};

TEGRA210_UBOOT_CLOCK_MANAGEMENT_PROTOCOL mUbootClockProto = {
    UbClkGetRate,
    UbClkSetRate,
    UbEnableClk,
    UbDisableClk,
    UbResetAssert,
    UbResetDeassert,
    UbResetPeriph,
    UbGetOscFreq,
    UbStartPll
};

STATIC
EFI_STATUS
EFIAPI
EnableClDvfs
(
    VOID
)
{
    clock_enable_cl_dvfs();
    return EFI_SUCCESS;
}

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

STATIC
BOOLEAN
EFIAPI
SdMmcClockNotResetAndEnabled
(
    UINT32 Index
)
{
    return clock_sdmmc_is_not_reset_and_enabled(Index);
}

STATIC
EFI_STATUS
EFIAPI
SdMmcClockGetParams
(
    UINT32* pOut,
    UINT16* pDivisor,
    UINT32  Type
)
{
    clock_sdmmc_get_params(pOut, pDivisor, Type);
    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SdMmcClockEnable
(
    UINT32  Index,
    UINT32  Clock
)
{
    clock_sdmmc_enable(Index, Clock);
    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SdMmcClockDisable
(
    UINT32 Index
)
{
    clock_sdmmc_disable(Index);
    return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SdMmcClockConfigSource
(
    UINT32 *pOut,
    UINT32 DeviceId,
    UINT32 Value
)
{
    clock_sdmmc_config_clock_source(pOut, DeviceId, Value);
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

    UbInitialize();

    Status = gBS->InstallMultipleProtocolInterfaces(
        &ProtoHandle,
        &gTegra210ClockManagementProtocolGuid,
        &mClockProto,
        &gTegraUBootClockManagementProtocolGuid,
        &mUbootClockProto,
        NULL
    );

    ASSERT_EFI_ERROR(Status);
    return Status;
}
