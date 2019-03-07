#ifndef __EFI_SDMMC_BLK_INTERNAL_PROTO_H__
#define __EFI_SDMMC_BLK_INTERNAL_PROTO_H__

#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/BlockIo.h>

//
// Device structures
//
typedef struct {
    VENDOR_DEVICE_PATH  Mmc;
    EFI_DEVICE_PATH     End;
} MMCHS_DEVICE_PATH;

typedef struct {
    UINT32                                Signature;
    EFI_HANDLE                            Handle;
    EFI_BLOCK_IO_PROTOCOL                 BlockIo;
    EFI_BLOCK_IO_MEDIA                    BlockMedia;
    MMCHS_DEVICE_PATH                     DevicePath;
} BIO_INSTANCE;

#define BIO_INSTANCE_SIGNATURE SIGNATURE_32('e', 'm', 'm', 'c')
#define BIO_INSTANCE_FROM_BLOCKIO_THIS(a) CR(a, BIO_INSTANCE, BlockIo, BIO_INSTANCE_SIGNATURE)

//
// Function Prototypes
//

EFI_STATUS
EFIAPI
MMCHSReset(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN BOOLEAN                        ExtendedVerification
);

EFI_STATUS
EFIAPI
MMCHSReadBlocks(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN UINT32                         MediaId,
    IN EFI_LBA                        Lba,
    IN UINTN                          BufferSize,
    OUT VOID                          *Buffer
);

EFI_STATUS
EFIAPI
MMCHSWriteBlocks(
    IN EFI_BLOCK_IO_PROTOCOL          *This,
    IN UINT32                         MediaId,
    IN EFI_LBA                        Lba,
    IN UINTN                          BufferSize,
    IN VOID                           *Buffer
);

EFI_STATUS
EFIAPI
MMCHSFlushBlocks(
    IN EFI_BLOCK_IO_PROTOCOL  *This
);

EFI_STATUS
BioInstanceContructor(
    OUT BIO_INSTANCE** NewInstance
);

#endif