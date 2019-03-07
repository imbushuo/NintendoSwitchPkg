#ifndef __SHIM_DEBUGLIB_H__
#define __SHIM_DEBUGLIB_H__

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#if __DEBUG__REQUIRED__
#define debug(...) DEBUG((EFI_D_INFO, __VA_ARGS__))
#else
#define debug(...) 
#endif
#define assert(...) ASSERT(__VA_ARGS__)
#define printf(...) DEBUG((EFI_D_INFO, __VA_ARGS__))

#endif
