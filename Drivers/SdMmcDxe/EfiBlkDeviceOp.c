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
#include <Device/StaticDevices.h>
#include <Foundation/Types.h>

#include "Include/SdMmc.h"
#include "Include/HostOp.h"
#include "Include/EfiProto.h"

STATIC BIO_INSTANCE mBioTemplate = {
    BIO_INSTANCE_SIGNATURE,
    NULL, // Handle
    { 
        // BlockIo
        EFI_BLOCK_IO_INTERFACE_REVISION,   // Revision
        NULL,                              // *Media
        MMCHSReset,                        // Reset
        MMCHSReadBlocks,                   // ReadBlocks
        MMCHSWriteBlocks,                  // WriteBlocks
        MMCHSFlushBlocks                   // FlushBlocks
    },
    { 
        // BlockMedia
        BIO_INSTANCE_SIGNATURE,                   // MediaId
        FALSE,                                    // RemovableMedia
        TRUE,                                     // MediaPresent
        FALSE,                                    // LogicalPartition
        TRUE,                                     // ReadOnly
        FALSE,                                    // WriteCaching
        0,                                        // BlockSize
        4,                                        // IoAlign
        0,                                        // Pad
        0                                         // LastBlock
    },
    {   // DevicePath
        {
            {
                HARDWARE_DEVICE_PATH, HW_VENDOR_DP,
                { (UINT8) (sizeof(VENDOR_DEVICE_PATH)), (UINT8) ((sizeof(VENDOR_DEVICE_PATH)) >> 8) },
            },
            // Hardware Device Path for Bio
            EFI_CALLER_ID_GUID // Use the driver's GUID
        },
        {
            END_DEVICE_PATH_TYPE,
            END_ENTIRE_DEVICE_PATH_SUBTYPE,
            { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
        }
    }
};

EFI_STATUS
EFIAPI
MMCHSReset(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN BOOLEAN                        ExtendedVerification
)
{
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
MMCHSReadBlocks(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN UINT32                         MediaId,
    IN EFI_LBA                        Lba,
    IN UINTN                          BufferSize,
    OUT VOID                          *Buffer
)
{
    BIO_INSTANCE              *Instance;
    EFI_BLOCK_IO_MEDIA        *Media;
    UINTN                     BlockSize;
    UINTN                     rc;

    Instance  = BIO_INSTANCE_FROM_BLOCKIO_THIS(This);
    Media     = &Instance->BlockMedia;
    BlockSize = Media->BlockSize;

    if (MediaId != Media->MediaId) 
    {
        return EFI_MEDIA_CHANGED;
    }

    if (Lba > Media->LastBlock) 
    {
        return EFI_INVALID_PARAMETER;
    }

    if ((Lba + (BufferSize / BlockSize) - 1) > Media->LastBlock) 
    {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize % BlockSize != 0) 
    {
        return EFI_BAD_BUFFER_SIZE;
    }

    if (Buffer == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize == 0) 
    {
        return EFI_SUCCESS;
    }

    rc = mmc_bread(Lba, BufferSize / BlockSize, Buffer);
    if (rc) return EFI_SUCCESS;

    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
MMCHSWriteBlocks(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN UINT32                         MediaId,
    IN EFI_LBA                        Lba,
    IN UINTN                          BufferSize,
    IN VOID                           *Buffer
)
{
    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
MMCHSFlushBlocks(
    IN EFI_BLOCK_IO_PROTOCOL  *This
)
{
    return EFI_SUCCESS;
}

EFI_STATUS
BioInstanceContructor(
    OUT BIO_INSTANCE** NewInstance
)
{
    BIO_INSTANCE* Instance;

    Instance = AllocateCopyPool(sizeof(BIO_INSTANCE), &mBioTemplate);
    if (Instance == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Instance->BlockIo.Media = &Instance->BlockMedia;

    *NewInstance = Instance;
    return EFI_SUCCESS;
}
