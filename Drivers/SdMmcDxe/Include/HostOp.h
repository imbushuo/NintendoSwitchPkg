#ifndef __HOSTOP_H__
#define __HOSTOP_H__

#include <Uefi.h>
EFIAPI
int
SdFxInit(
    VOID
);

EFIAPI
int
SdFxInitFinalize
(
	VOID
);

ulong mmc_bread(UINT64 start, UINT64 blkcnt, void *dst);

#endif