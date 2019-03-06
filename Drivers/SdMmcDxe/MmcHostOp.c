#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DevicePath.h>

#include <Protocol/UBootClockManagement.h>
#include <Protocol/Utc/Clock.h>
#include <Protocol/Utc/ErrNo.h>

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Device/Pmc.h>
#include <Library/GpioLib.h>
#include <Protocol/Pmic.h>
#include <Shim/DebugLib.h>
#include <Shim/UBootIo.h>
#include <Shim/TimerLib.h>
#include <Shim/BitOps.h>
#include <Shim/MemAlign.h>
#include <Library/Utc/BounceBuf.h>

#include "Include/TegraMmc.h"

extern MMC_CONFIG mConfig;
extern TEGRA_MMC_PRIV mPriv;

EFIAPI
VOID
SdFxStart(
    VOID
)
{
    int err, i;
	uint mult, freq;
	u64 cmult, csize, capacity;
	struct mmc_cmd cmd;
    ALLOC_CACHE_ALIGN_BUFFER(u8, ext_csd, MMC_MAX_BLOCK_LEN);
	ALLOC_CACHE_ALIGN_BUFFER(u8, test_csd, MMC_MAX_BLOCK_LEN);
	bool has_parts = false;
	bool part_completed;

    /* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;

    err = tegra_mmc_send_cmd(&mPriv, &cmd, NULL);
    if (err)
    {
        DEBUG((EFI_D_ERROR, "Failed to put in identify mode, error %d \n", err));
        ASSERT(FALSE);
    }

    DEBUG((EFI_D_INFO, "Command succeeded \n"));
}