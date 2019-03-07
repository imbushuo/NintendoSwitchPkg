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

ulong mmc_bread(lbaint_t start, lbaint_t blkcnt, void *dst);

#endif