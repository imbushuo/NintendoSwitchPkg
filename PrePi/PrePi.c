/** @file
*
*  Copyright (c) 2011-2017, ARM Limited. All rights reserved.
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

#include <PiPei.h>

#include <Library/DebugAgentLib.h>
#include <Library/PrePiLib.h>
#include <Library/PrintLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Library/TimerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/CacheMaintenanceLib.h>

#include <Ppi/GuidedSectionExtraction.h>
#include <Ppi/ArmMpCoreInfo.h>
#include <Ppi/SecPerformance.h>

#include <FwReleaseInfo.h>
#include "PrePi.h"

#define IS_XIP() (((UINT64)FixedPcdGet64 (PcdFdBaseAddress) > mSystemMemoryEnd) || \
                  ((FixedPcdGet64 (PcdFdBaseAddress) + FixedPcdGet32 (PcdFdSize)) < FixedPcdGet64 (PcdSystemMemoryBase)))

UINT64 mSystemMemoryEnd = FixedPcdGet64(PcdSystemMemoryBase) +
                          FixedPcdGet64(PcdSystemMemorySize) - 1;

STATIC ARM_CORE_INFO mSwitchCoreInfoTable[] = {
  { 0x0, 0x0, },             // Cluster 0, Core 0
  { 0x0, 0x1, },             // Cluster 0, Core 1
  { 0x0, 0x2, },             // Cluster 0, Core 2
  { 0x0, 0x3, },             // Cluster 0, Core 3
};

VOID
BreakColor(
	VOID
)
{
	UINT32* FrameBuffer = (void*)FixedPcdGet32(PcdMipiFrameBufferAddress);
	for (UINTN i = 0; i < 720; i++)
	{
		for (UINTN j = 0; j < 50; j++)
		{
			*FrameBuffer = 0xFFFFFFFF;
			FrameBuffer++;
		}
	}

	while (TRUE) {}
}

VOID
Panic(
	VOID
)
{
	UINT32* FrameBuffer = (void*)FixedPcdGet32(PcdMipiFrameBufferAddress);
	for (UINTN i = 0; i < 720; i++)
	{
		for (UINTN j = 0; j < 50; j++)
		{
			*FrameBuffer = 0xFFBE4EE3;
			FrameBuffer++;
		}
	}

	while (TRUE) {}
}

VOID
PrePiMain (
  IN  UINTN                     UefiMemoryBase,
  IN  UINTN                     StackBase,
  IN  UINT64                    StartTimeStamp,
  IN  UINTN						MpId
  )
{
	EFI_HOB_HANDOFF_INFO_TABLE*   HobList;
	UINTN                         ArmCoreCount;
	EFI_STATUS                    Status;
	CHAR8                         Buffer[255];
	UINTN                         CharCount;
	FIRMWARE_SEC_PERFORMANCE      Performance;

	// Architecture-specific initialization
	// Enable Floating Point
	ArmEnableVFP();

	// Enable program flow prediction, if supported
	ArmEnableBranchPrediction();

	// Initialize the Serial Port
	SerialPortInitialize();

	CharCount = AsciiSPrint (
		Buffer,
		sizeof (Buffer), 
		"Nintendo Switch UEFI firmware on MPIDR 0x%llx, (version %s commit %a built at %a on %a)\n\r",
		MpId,
		(CHAR16*) PcdGetPtr(PcdFirmwareVersionString), 
		__IMPL_COMMIT_ID__, 
		__TIME__,
		__DATE__
	);

	SerialPortWrite ((UINT8 *) Buffer, CharCount);
	ASSERT(ArmPlatformIsPrimaryCore(MpId) != 0);

	// Declare the PI/UEFI memory region
	HobList = HobConstructor(
		(VOID*) UefiMemoryBase,
		FixedPcdGet32(PcdSystemMemoryUefiRegionSize),
		(VOID*) UefiMemoryBase,
		(VOID*) StackBase  // The top of the UEFI Memory is reserved for the stacks
	);
	PrePeiSetHobList(HobList);

	// Initialize MMU and Memory HOBs (Resource Descriptor HOBs)
	Status = MemoryPeim(UefiMemoryBase, FixedPcdGet32(PcdSystemMemoryUefiRegionSize));

	// Create the Stacks HOB (reserve the memory for all stacks)
	BuildStackHob(StackBase, FixedPcdGet32(PcdPrePiStackSize));

	// TODO: Call CpuPei as a library
	BuildCpuHob(PcdGet8(PcdPrePiCpuMemorySize), PcdGet8(PcdPrePiCpuIoSize));

	// Build the MP Core Info Table
	ArmCoreCount = FixedPcdGet32(PcdCoreCount);

	// Build MPCore Info HOB
	BuildGuidDataHob(&gArmMpCoreInfoGuid, &mSwitchCoreInfoTable, sizeof(ARM_CORE_INFO) * ArmCoreCount);

	// Store timer value logged at the beginning of firmware image execution
	Performance.ResetEnd = GetTimeInNanoSecond(StartTimeStamp);

	// Build SEC Performance Data Hob
	BuildGuidDataHob(&gEfiFirmwarePerformanceGuid, &Performance, sizeof(Performance));

	// Set the Boot Mode
	SetBootMode(ArmPlatformGetBootMode());

	// Initialize Platform HOBs (CpuHob and FvHob)
	Status = PlatformPeim();
	ASSERT_EFI_ERROR(Status);

	// Now, the HOB List has been initialized, we can register performance information
	PERF_START(NULL, "PEI", NULL, StartTimeStamp);

	// SEC phase needs to run library constructors by hand.
	ProcessLibraryConstructorList();

	// Assume the FV that contains the SEC (our code) also contains a compressed FV.
	Status = DecompressFirstFv();
	ASSERT_EFI_ERROR(Status);

	// Load the DXE Core and transfer control to it
	Status = LoadDxeCoreFromFv(NULL, 0);
	ASSERT_EFI_ERROR(Status);
}

VOID
CEntryPoint (
  IN  UINTN                     MpId,
  IN  UINTN                     UefiMemoryBase,
  IN  UINTN                     StacksBase
  )
{
	UINT64   StartTimeStamp;

	// Initialize the platform specific controllers
	ArmPlatformInitialize(MpId);

	if (ArmPlatformIsPrimaryCore(MpId) && PerformanceMeasurementEnabled())
	{
		// Initialize the Timer Library to setup the Timer HW controller
		TimerConstructor();

		// We cannot call yet the PerformanceLib because the HOB List has not been initialized
		StartTimeStamp = GetPerformanceCounter();
	}
	else 
	{
		StartTimeStamp = 0;
	}

	// Data Cache enabled on Primary core when MMU is enabled.
	ArmDisableDataCache();
	// Invalidate Data cache
	ArmInvalidateDataCache();
	// Invalidate instruction cache
	ArmInvalidateInstructionCache();
	// Enable Instruction Caches on all cores.
	ArmEnableInstructionCache();

	// Must be primary, lol
	// Goto primary Main.
	PrePiMain(UefiMemoryBase, StacksBase, StartTimeStamp, MpId);

	// DXE Core should always load and never return
	ASSERT(FALSE);
}
