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

#endif