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
#include <Protocol/Utc/Clock.h>
#include <Protocol/UBootClockManagement.h>
#include "Include/UBootClkImpl.h"

VOID
EFIAPI
UbEnableClk
(
    IN UINT64 ClkId
)
{
    clock_enable(ClkId);
}

VOID
EFIAPI
UbDisableClk
(
    IN UINT64 ClkId
)
{
    clock_disable(ClkId);
}

UINT64
EFIAPI
UbClkGetRate
(
    IN UINT64 ClkId
)
{
    enum clock_id parent;
	parent = clock_get_periph_parent(ClkId);
	return clock_get_periph_rate(ClkId, parent);
}

UINT64
EFIAPI
UbClkSetRate
(
    IN UINT64 ClkId,
    IN UINT64 Rate
)
{
    enum clock_id parent;
	parent = clock_get_periph_parent(ClkId);
	return clock_adjust_periph_pll_div(ClkId, parent, Rate, NULL);
}

VOID
EFIAPI
UbResetAssert
(
    IN UINT64 RstPId
)
{
    return reset_set_enable(RstPId, 1);
}

VOID
EFIAPI
UbResetDeassert
(
    IN UINT64 RstPId
)
{
    return reset_set_enable(RstPId, 0);
}

VOID
EFIAPI
UbResetPeriph
(
    IN UINT64 PeriphId,
    IN int Delay
)
{
    reset_periph(PeriphId, Delay);
}

enum clock_osc_freq
EFIAPI
UbGetOscFreq
(
    VOID
)
{
    return clock_get_osc_freq();
}

unsigned long
EFIAPI
UbStartPll
(
    enum clock_id clkid, 
    u32 divm, u32 divn,
	u32 divp, u32 cpcon, 
    u32 lfcon
)
{
    return clock_start_pll(clkid, divm, divn, divp, cpcon, lfcon);
}

VOID
EFIAPI
UbInitialize
(
    VOID
)
{
    clock_init();
    clock_verify();
}