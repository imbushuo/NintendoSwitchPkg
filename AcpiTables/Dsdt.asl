/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014-2018, ARM Ltd. All rights reserved.<BR>
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Platform.h"

DefinitionBlock("DsdtTable.aml", "DSDT", 1, "NVIDIA", "NINTENDO", EFI_ACPI_ARM_OEM_REVISION) 
{
  Scope(_SB) {
    //
    // A57x4 Processor declaration
    //
    Method (_OSC, 4, Serialized)  { // _OSC: Operating System Capabilities
      CreateDWordField (Arg3, 0x00, STS0)
      CreateDWordField (Arg3, 0x04, CAP0)
      Return (Arg3)
    }

    Device (CLU0) { // Cluster0 state
      Name(_HID, "ACPI0010")
      Name(_UID, 1)

      Device(CPU0) { // A57-0: Cluster 0, Cpu 0
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
      }
      Device(CPU1) { // A57-0: Cluster 0, Cpu 1
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
      }
      Device(CPU2) { // A57-0: Cluster 0, Cpu 2
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
      }
      Device(CPU3) { // A57-0: Cluster 0, Cpu 3
        Name(_HID, "ACPI0007")
        Name(_UID, 0)
      }
    }
  } // Scope(_SB)
}
