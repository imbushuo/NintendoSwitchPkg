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

STATIC UINTN mTotalMemorySize = 0;

STATIC UINTN QueryMemorySize(
	VOID
)
{
	struct mc_ctlr *mc = (struct mc_ctlr *) NV_PA_MC_BASE;
	u32 emem_cfg;
	UINTN size_bytes;

	emem_cfg = readl(&mc->mc_emem_cfg);

	/* RAM size EMC is programmed to. */
	size_bytes = (UINTN) emem_cfg * 1024 * 1024;

	return size_bytes;
}

VOID
ArmPlatformGetVirtualMemoryMap(
	IN ARM_MEMORY_REGION_DESCRIPTOR** VirtualMemoryMap
)
{
	ASSERT(FALSE);
}