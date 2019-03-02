#ifndef __STATIC_DEVICES_H__
#define __STATIC_DEVICES_H__

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/DevicePathLib.h>

#define PLAT_SBB_DEVICE_GUID \
    { 0x2abf720d, 0x6e2a, 0x4ca8, { 0x80, 0x96, 0x31, 0x7e, 0xf8, 0x27, 0x5b, 0x60} }

#define PLAT_SDMMC_SLOT1_DEVICE_GUID \
    { 0x2abf720d, 0x6e2a, 0x4ca9, { 0x80, 0x96, 0x31, 0x7e, 0xf8, 0x27, 0x5b, 0x01} }

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH  Device;
  EFI_DEVICE_PATH     End;
} STATIC_DEVICE_PATH;
#pragma pack()

STATIC_DEVICE_PATH gSbbDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
      },
    },
    PLAT_SBB_DEVICE_GUID,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
  }
};

STATIC_DEVICE_PATH gSdmmcHost1DevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
      },
    },
    PLAT_SDMMC_SLOT1_DEVICE_GUID,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
  }
};

#endif