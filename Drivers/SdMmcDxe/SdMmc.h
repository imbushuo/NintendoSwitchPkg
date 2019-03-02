#ifndef __SDMMC_H__
#define __SDMMC_H__

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
  sdmmc_storage_t                       *MmcDev;
} BIO_INSTANCE;

#define BIO_INSTANCE_SIGNATURE  SIGNATURE_32('s', 'd', 'h', 'c')
#define BIO_INSTANCE_FROM_BLOCKIO_THIS(a)    CR(a, BIO_INSTANCE, BlockIo, BIO_INSTANCE_SIGNATURE)

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

#define SDHCI_ADMA_DESC_LINE_SZ                   65536
#define SDHCI_ADMA_MAX_TRANS_SZ (65535 * 512)

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#endif