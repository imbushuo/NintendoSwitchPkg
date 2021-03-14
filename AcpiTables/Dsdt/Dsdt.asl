/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014-2018, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2019-2020, Bingxing Wang. All rights reserved.<BR>
  Portion retrieved from Surface RT 2 DSDT dump

  This program and the accompanying materials are licensed and made available under 
  the terms and conditions of the BSD License which accompanies this distribution.  
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Platform.h"

DefinitionBlock("DsdtTable.aml", "DSDT", 1, "NVIDIA", "TEGRA210", EFI_ACPI_ARM_OEM_REVISION) 
{
    Scope(_SB) 
    {

        Device(CPU0) 
        {
            Name(_HID, "ACPI0007")
            Name(_UID, 0)
        }

        Device(CPU1) 
        {
            Name(_HID, "ACPI0007")
            Name(_UID, 1)
        }

        Device(CPU2) 
        {
            Name(_HID, "ACPI0007")
            Name(_UID, 2)
        }

        Device(CPU3) 
        {
            Name(_HID, "ACPI0007")
            Name(_UID, 3)
        }

        // Helper functions
        Method (GCRS, 3, NotSerialized)
        {
            Name (RSRC, ResourceTemplate ()
            {
                Memory32Fixed (ReadWrite,
                    0x00000000,         // Address Base
                    0x00000000,         // Address Length
                    _Y00)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive, ,, _Y01)
                {
                    0x00000000,
                }
            })
            CreateDWordField (RSRC, \_SB.GCRS._Y00._BAS, MBAS)  // _BAS: Base Address
            CreateDWordField (RSRC, \_SB.GCRS._Y00._LEN, MBLE)  // _LEN: Length
            CreateWordField (RSRC, \_SB.GCRS._Y01._INT, INTN)  // _INT: Interrupts
            MBAS = Arg0
            MBLE = Arg1
            INTN = Arg2
            Return (RSRC)
        }

        Method (CRSD, 2, NotSerialized)
        {
            Name (RSRC, ResourceTemplate ()
            {
                FixedDMA (0x0000, 0x0000, Width32bit, _Y07)
                FixedDMA (0x0001, 0x0001, Width32bit, _Y08)
            })
            CreateWordField (RSRC, One, REQ1)
            CreateWordField (RSRC, \_SB.CRSD._Y07._TYP, CHN1)  // _TYP: Type
            CreateWordField (RSRC, \_SB.CRSD._Y08._DMA, REQ2)  // _DMA: Direct Memory Access
            CreateWordField (RSRC, \_SB.CRSD._Y08._TYP, CHN2)  // _TYP: Type
            REQ1 = Arg0
            REQ2 = (Arg0 | 0x20)
            CHN1 = Arg1
            CHN2 = Arg1++
            Return (RSRC)
        }

        Method (CRS3, 5, NotSerialized)
        {
            Local0 = GCRS (Arg0, Arg1, Arg2)
            Local1 = CRSD (Arg3, Arg4)
            Return (ConcatenateResTemplate (Local0, Local1))
        }

        // UART 2: Right Joy-Con
        Device (UAR2)
        {
            Method (_HID, 0, NotSerialized)
            {
                Return ("NVDA0100")
            }

            Name (_UID, One)
            Method (_CRS, 0, NotSerialized)
            {
                Return (CRS3 (0x70006040, 0x40, 0x45, 0x09, 0x02))
            }

            Method (_STA, 0, NotSerialized)
            {
                Return (0x0F)
            }
        }

    } // Scope(_SB)
}
