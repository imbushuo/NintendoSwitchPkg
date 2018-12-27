/** @file
*
*  Copyright (c) 2010-2015, NVIDIA Corporation. All rights reserved.
*  Copyright (c) 2014, Linaro Limited. All rights reserved.
*  Copyright (c), 2017-2018, Andrey Warkentin <andrey.warkentin@gmail.com>
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Library/BaseLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Foundation/Types.h>
#include <Device/T210.h>

VOID
ArmPlatformGetVirtualMemoryMap(
	IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
)
{
	// You are not expected to call this
	ASSERT(FALSE);
}
