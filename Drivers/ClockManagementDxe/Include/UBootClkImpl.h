#ifndef __UBOOT_CLK_IMPL_H__
#define __UBOOT_CLK_IMPL_H__

VOID
EFIAPI
UbEnableClk
(
    IN UINT64 ClkId
);

VOID
EFIAPI
UbDisableClk
(
    IN UINT64 ClkId
);

UINT64
EFIAPI
UbClkGetRate
(
    IN UINT64 ClkId
);

UINT64
EFIAPI
UbClkSetRate
(
    IN UINT64 ClkId,
    IN UINT64 Rate
);

VOID
EFIAPI
UbResetAssert
(
    IN UINT64 RstPId
);

VOID
EFIAPI
UbResetDeassert
(
    IN UINT64 RstPId
);

VOID
EFIAPI
UbResetPeriph
(
    IN UINT64 PeriphId,
    IN int Delay
);

enum clock_osc_freq
EFIAPI
UbGetOscFreq
(
    VOID
);

unsigned long
EFIAPI
UbStartPll
(
    enum clock_id clkid, 
    u32 divm, u32 divn,
	u32 divp, u32 cpcon, 
    u32 lfcon
);

VOID
EFIAPI
UbInitialize
(
    VOID
);

#endif