/** @file
*
*  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
*  Copyright (c) 2014, Red Hat, Inc.
*  Copyright (c) 2014-2016, Linaro Limited. All rights reserved.
*
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

#include <Library/IoLib.h>
#include <Library/ArmPlatformLib.h>
#include <Library/DebugLib.h>
#include <Ppi/ArmMpCoreInfo.h>

/**
  Return the current Boot Mode

  This function returns the boot reason on the platform

  @return   Return the current Boot Mode of the platform

**/
EFI_BOOT_MODE
ArmPlatformGetBootMode(
	VOID
)
{
	return BOOT_WITH_FULL_CONFIGURATION;
}

/**
  This function is called by PrePeiCore, in the SEC phase.
**/
RETURN_STATUS
ArmPlatformInitialize(
	IN  UINTN                     MpId
)
{
	return RETURN_SUCCESS;
}

VOID
ArmPlatformInitializeSystemMemory(
	VOID
)
{
}

ARM_CORE_INFO mSwitchCoreInfoTable[] = {
  { 0x0, 0x100, },             // Cluster 0, Core 0
  { 0x0, 0x101, },             // Cluster 0, Core 1
  { 0x0, 0x102, },             // Cluster 0, Core 2
  { 0x0, 0x103, },             // Cluster 0, Core 3
};

STATIC
EFI_STATUS
PrePeiCoreGetMpCoreInfo(
	OUT UINTN                   *CoreCount,
	OUT ARM_CORE_INFO           **ArmCoreTable
)
{
	// Only support one cluster
	*CoreCount = sizeof(mSwitchCoreInfoTable) / sizeof(ARM_CORE_INFO);
	*ArmCoreTable = mSwitchCoreInfoTable;

	return EFI_SUCCESS;
}

// Needs to be declared in the file. Otherwise gArmMpCoreInfoPpiGuid is undefined in the contect of PrePeiCore
EFI_GUID mArmMpCoreInfoPpiGuid = ARM_MP_CORE_INFO_PPI_GUID;
ARM_MP_CORE_INFO_PPI mMpCoreInfoPpi = { PrePeiCoreGetMpCoreInfo };

EFI_PEI_PPI_DESCRIPTOR gPlatformPpiTable[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &mArmMpCoreInfoPpiGuid,
    &mMpCoreInfoPpi
  }
};

VOID
ArmPlatformGetPlatformPpiList(
	OUT UINTN                   *PpiListSize,
	OUT EFI_PEI_PPI_DESCRIPTOR  **PpiList
)
{
	*PpiListSize = sizeof(gPlatformPpiTable);
	*PpiList = gPlatformPpiTable;
}
