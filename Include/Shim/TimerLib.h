#ifndef __SHIM_TIMERLIB_H__
#define __SHIM_TIMERLIB_H__

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TimerLib.h>

#define udelay(x) gBS->Stall((x))
#define get_timer(x) ((GetTimeInNanoSecond(GetPerformanceCounter()) / 1000) - (x))

inline UINT64 timer_get_us()
{
    return GetTimeInNanoSecond(GetPerformanceCounter()) / 1000;
}

#endif