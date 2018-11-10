//
//  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
//
//  This program and the accompanying materials
//  are licensed and made available under the terms and conditions of the BSD License
//  which accompanies this distribution.  The full text of the license may be found at
//  http://opensource.org/licenses/bsd-license.php
//
//  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//

#include <Library/PcdLib.h>
#include <AsmMacroIoLibV8.h>

#define LoadConstantToReg(Data, Reg) \
  ldr  Reg, =Data

.text
.align 3

GCC_ASM_IMPORT (CEntryPoint)
GCC_ASM_EXPORT (_ModuleEntryPoint)
GCC_ASM_IMPORT (ArmEnableInstructionCache)
GCC_ASM_IMPORT (ArmEnableDataCache)
GCC_ASM_IMPORT (ArmInvalidateTlb)

.global _StackBase 
.global _StackSize
.global _UefiMemoryBase

_StackBase:
  .quad FixedPcdGet64(PcdPrePiStackBase)

_StackSize:
  .quad FixedPcdGet64(PcdPrePiStackSize)

_UefiMemoryBase:
  .quad FixedPcdGet64(PcdSystemMemoryBase)

_ModuleEntryPoint:
  mov   x0, #0

  // First ensure all interrupts are disabled
  bl    ASM_PFX(ArmDisableInterrupts)

  // Ensure that the MMU and caches are off
  bl    ASM_PFX(ArmDisableCachesAndMmu)
 
  // Invalide I-Cache
  bl    ASM_PFX(ArmInvalidateInstructionCache)
  
  // Invalidate TLB
  bl    ASM_PFX(ArmInvalidateTlb)

  // Do early platform specific actions
  bl    ASM_PFX(ArmPlatformPeiBootAction)

  // Get ID of this CPU in Multicore system
  bl    ASM_PFX(ArmReadMpidr)

  // Keep a copy of the MpId register value
  mov   x10, x0

_SetupExceptionVector:
  LoadConstantToReg (FixedPcdGet32(PcdCpuVectorBaseAddress), x0)
  ldr   x1, _PointDead
  mov   x2, #0

_FillVectors:
  str   x1, [x0, x2]
  adds  x2, x2, #8      /* Fill every 8 byte */
  cmp   x2, #0x800      /* Vector Table Size */
  bne   _FillVectors
  
  /* Update VBAR */
  msr   vbar_el2, x0    /* End _SetupExceptionVector */

_SetupStack:
  ldr   x13, _StackBase     /* Stack base arg0 */
  ldr   x14, _StackSize     /* Stack size arg1 */
  
  add   x4, x13, x14
  add   sp, x4, #0

_EnableCache: 
  bl    ArmInvalidateDataCache
  bl    ASM_PFX(ArmEnableInstructionCache)
  bl    ASM_PFX(ArmEnableDataCache)

_PrepareArguments:
  ldr   x12, _UefiMemoryBase

  mov   x0, x10
  mov   x1, x12
  mov   x2, x13

  bl    CEntryPoint

_NeverReturn:
  b     _NeverReturn

.align 3
_PointDead:
  // This is probably some weird diagnostcs
  mov   x1, #0xffffffff
  mov   x2, #0xc0000000
  str   x1, [x2]
  b     _PointDead

