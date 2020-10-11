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
#include <Library/Max7762xLib.h>
#include <Library/GpioLib.h>
#include <Protocol/Pmic.h>
#include <Device/StaticDevices.h>

#define SBB_POLL_INTERVAL 100000

EFI_EVENT m_CallbackTimer;
PMIC_PROTOCOL *mPmicProtocol;

VOID
EFIAPI SidebandButtonsPoll(
	IN  EFI_EVENT   Event,
	IN  VOID        *Context
)
{
    if (!gpio_read(GPIO_BY_NAME(BUTTON_VOL_DOWN)))
    {
        // DEBUG((EFI_D_ERROR | EFI_D_INFO, "Volume down button pressed \n"));
    }
    
    if (!gpio_read(GPIO_BY_NAME(BUTTON_VOL_UP)))
    {
        // DEBUG((EFI_D_ERROR | EFI_D_INFO, "Volume up button pressed \n"));
    }

    if (mPmicProtocol->QueryPowerButton())
    {
        // DEBUG((EFI_D_ERROR | EFI_D_INFO, "Power button pressed \n"));
    }
}

EFI_STATUS
EFIAPI
SidebandButtonDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;

    // Locate PMIC protocol.
    Status = gBS->LocateProtocol(
        &gPmicProtocolGuid,
        NULL,
        (VOID**) &mPmicProtocol
    );
    if (EFI_ERROR(Status)) goto exit;

    // Create polling.
    Status = gBS->CreateEvent(
        EVT_NOTIFY_SIGNAL | EVT_TIMER,
        TPL_CALLBACK,
        SidebandButtonsPoll,
        NULL,
        &m_CallbackTimer
    );

    if (EFI_ERROR(Status)) goto exit;

    Status = gBS->SetTimer(
        m_CallbackTimer,
        TimerPeriodic,
        SBB_POLL_INTERVAL
    );

    if (EFI_ERROR(Status)) goto exit;

exit:
    ASSERT_EFI_ERROR(Status);
    return Status;
}
