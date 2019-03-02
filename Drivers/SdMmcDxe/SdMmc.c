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
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DevicePath.h>
#include <Device/StaticDevices.h>

#include "Library/sdmmc.h"
#include "SdMmc.h"

#include <Foundation/Types.h>
#include <Device/T210.h>
#include <Library/Max7762xLib.h>
#include <Library/GpioLib.h>
#include <Library/PinmuxLib.h>

sdmmc_t mController;
sdmmc_storage_t mStorage;

STATIC BIO_INSTANCE mBioTemplate = {
  BIO_INSTANCE_SIGNATURE,
  NULL, // Handle
  { // BlockIo
    EFI_BLOCK_IO_INTERFACE_REVISION,   // Revision
    NULL,                              // *Media
    MMCHSReset,                        // Reset
    MMCHSReadBlocks,                   // ReadBlocks
    MMCHSWriteBlocks,                  // WriteBlocks
    MMCHSFlushBlocks                   // FlushBlocks
  },
  { // BlockMedia
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
  { // DevicePath
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

/*
 * Function: mmc_write
 * Arg     : Data address on card, data length, i/p buffer
 * Return  : 0 on Success, non zero on failure
 * Flow    : Write the data from in to the card
 */
STATIC UINT32 mmc_write(BIO_INSTANCE *Instance, UINT64 data_addr, UINT32 data_len, VOID *in)
{
    return 0;
}

/*
 * Function: mmc_read
 * Arg     : Data address on card, o/p buffer & data length
 * Return  : 0 on Success, non zero on failure
 * Flow    : Read data from the card to out
 */
STATIC UINT32 mmc_read(BIO_INSTANCE *Instance, UINT64 data_addr, UINT32 *out, UINT32 data_len)
{
    UINT32 ret = 0;
    UINT32 block_size;
    UINT32 read_size = 512;
    UINT8 *sptr = (UINT8 *)out;

    block_size = Instance->BlockMedia.BlockSize;

    ASSERT(!(data_addr % block_size));
    ASSERT(!(data_len % block_size));

    /*
    * dma onto write back memory is unsafe/nonportable,
    * but callers to this routine normally provide
    * write back buffers. Invalidate cache
    * before read data from mmc.
    */
    WriteBackInvalidateDataCacheRange(out, data_len);

    /* TODO: This function is aware of max data that can be
    * tranferred using sdhci adma mode, need to have a cleaner
    * implementation to keep this function independent of sdhci
    * limitations
    */
    while (data_len > read_size) 
    {
        ret = sdmmc_storage_read(Instance->MmcDev, data_addr / block_size, read_size / block_size, (VOID *) sptr);
        if (!ret)
        {
            DEBUG((EFI_D_ERROR, "Failed Reading block @ %x\n",(UINTN) (data_addr / block_size)));
            return 0;
        }
        sptr += read_size;
        data_addr += read_size;
        data_len -= read_size;
    }

    if (data_len)
    {
        ret = sdmmc_storage_read(Instance->MmcDev, data_addr / block_size, read_size / block_size, (VOID *) sptr);

        if (!ret)
        {
            DEBUG((EFI_D_ERROR, "Failed Reading block @ %x\n",(UINTN) (data_addr / block_size)));\
            return 0;
        }
    }

    return 1;
}

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
    UINTN                     RC;

    Instance  = BIO_INSTANCE_FROM_BLOCKIO_THIS(This);
    Media     = &Instance->BlockMedia;
    BlockSize = Media->BlockSize;

    if (MediaId != Media->MediaId) {
        return EFI_MEDIA_CHANGED;
    }

    if (Lba > Media->LastBlock) {
        return EFI_INVALID_PARAMETER;
    }

    if ((Lba + (BufferSize / BlockSize) - 1) > Media->LastBlock) {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize % BlockSize != 0) {
        return EFI_BAD_BUFFER_SIZE;
    }

    if (Buffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize == 0) {
        return EFI_SUCCESS;
    }

    RC = mmc_read(Instance, (UINT64) Lba * BlockSize, Buffer, BufferSize);
    if (RC) return EFI_SUCCESS;
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

    Instance = AllocateCopyPool (sizeof(BIO_INSTANCE), &mBioTemplate);
    if (Instance == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    Instance->BlockIo.Media = &Instance->BlockMedia;

    *NewInstance = Instance;
    return EFI_SUCCESS;
}

STATIC
VOID
InitializePinmux(
    VOID
)
{
    // Configure SD card detect pin
	pinmux_set_config(PINMUX_GPIO_Z1, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP | PINMUX_GPIO_PZ1_FUNC_SDMMC1);
	gpio_config(GPIO_DECOMPOSE(GPIO_Z1_INDEX), GPIO_MODE_GPIO);
	gpio_output_enable(GPIO_DECOMPOSE(GPIO_Z1_INDEX), GPIO_OUTPUT_DISABLE);
	APB_MISC(APB_MISC_GP_VGPIO_GPIO_MUX_SEL) = 0; // Use GPIO for all SDMMC 

	// Configure SD power enable pin (powered off by default)
	pinmux_set_config(PINMUX_DMIC3_CLK_INDEX, PINMUX_INPUT_ENABLE | PINMUX_PULL_DOWN | PINMUX_DMIC3_CLK_FUNC_I2S5A); //not sure about the altfunc here
	gpio_config(GPIO_BY_NAME(DMIC3_CLK), GPIO_MODE_GPIO);
	gpio_write(GPIO_BY_NAME(DMIC3_CLK), GPIO_LOW);
	gpio_output_enable(GPIO_BY_NAME(DMIC3_CLK), GPIO_OUTPUT_ENABLE);
}

EFI_STATUS
EFIAPI
SdMmcDxeInitialize
(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS Status;
    BIO_INSTANCE *Instance;

    InitializePinmux();
    if (sdmmc_storage_init_sd(&mStorage, &mController, SDMMC_1, SDMMC_BUS_WIDTH_4, 11))
    {
        Status = BioInstanceContructor(&Instance);
        if (EFI_ERROR(Status)) goto exit;

        Instance->MmcDev = &mStorage;
        Instance->BlockMedia.BlockSize = 512;
        Instance->BlockMedia.LastBlock = mStorage.csd.capacity;
        
        Status = gBS->InstallMultipleProtocolInterfaces(
            &Instance->Handle,
            &gEfiBlockIoProtocolGuid,    &Instance->BlockIo,
            &gEfiDevicePathProtocolGuid, &Instance->DevicePath,
            NULL
        );

        ASSERT_EFI_ERROR(Status);
    }
    else
    {
        Status = EFI_DEVICE_ERROR;
        goto exit;
    }
    
exit:
    return Status;
}