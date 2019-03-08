/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014-2018, ARM Ltd. All rights reserved.<BR>
  Portion retrieved from Surface RT 2 DSDT dump

  This program and the accompanying materials are licensed and made available under 
  the terms and conditions of the BSD License which accompanies this distribution.  
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Platform.h"

DefinitionBlock("DsdtTable.aml", "DSDT", 1, "NVIDIA", "NINTENDO", EFI_ACPI_ARM_OEM_REVISION) 
{
    Scope(_SB) 
    {
        //
        // A57x4 Processor declaration
        //
        Method (_OSC, 4, Serialized)  
        { 
          // _OSC: Operating System Capabilities
            CreateDWordField (Arg3, 0x00, STS0)
            CreateDWordField (Arg3, 0x04, CAP0)
            Return (Arg3)
        }

        Name (CHID, Zero)
        Name (TOOS, Zero)
        Method (_INI, 0, NotSerialized)  // _INI: Initialize
        {
            ^CHID = RDR (0x70000000, 0x0804)
            If (CondRefOf (\_OSI))
            {
                If (\_OSI ("Windows 2013"))
                {
                    ^TOOS = One
                }
            }
        }

        Method (RDR, 2, Serialized)
        {
            OperationRegion (GENO, SystemMemory, (Arg0 + Arg1), 0x04)
            Field (GENO, DWordAcc, NoLock, Preserve)
            {
                RD32,   32
            }

            Return (RD32) /* \_SB_.RDR_.RD32 */
        }

        Method (RDF, 4, Serialized)
        {
            Return (((RDR (Arg0, Arg1) & (Arg2 << Arg3)) >> Arg3))
        }

        Method (UCRS, 4, NotSerialized)
        {
            Name (RSRC, ResourceTemplate ()
            {
                Memory32Fixed (ReadWrite,
                    0x00000000,         // Address Base
                    0x00000000,         // Address Length
                    _Y02)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Shared, ,, _Y03)
                {
                    0x00000000,
                }
            })
            CreateDWordField (RSRC, \_SB.UCRS._Y02._BAS, MBAS)  // _BAS: Base Address
            CreateDWordField (RSRC, \_SB.UCRS._Y02._LEN, MBLE)  // _LEN: Length
            CreateWordField (RSRC, \_SB.UCRS._Y03._INT, INTN)  // _INT: Interrupts
            CreateField (RSRC, \_SB.UCRS._Y03._SHR, 0x02, SHRN)  // _SHR: Shareable
            MBAS = Arg0
            MBLE = Arg1
            INTN = Arg2
            SHRN = Arg3
            Return (RSRC) /* \_SB_.UCRS.RSRC */
        }

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
            Return (RSRC) /* \_SB_.GCRS.RSRC */
        }

        OperationRegion (CLKR, SystemMemory, 0x60006000, 0x1000)
        Field (CLKR, ByteAcc, NoLock, Preserve)
        {
            Offset (0x04), 
                ,   6, 
            UAAE,   1, 
            UABE,   1, 
            GPIE,   1, 
            SD2E,   1, 
                ,   1, 
            I2SE,   1, 
            I21E,   1, 
                ,   1, 
            SD1E,   1, 
            SD4E,   1, 
                ,   6, 
            US1E,   1, 
                ,   3, 
            Offset (0x08), 
                ,   9, 
            SP1E,   1, 
                ,   2, 
            SP2E,   1, 
                ,   1, 
            SP3E,   1, 
            I24E,   1, 
                ,   6, 
            I22E,   1, 
            UACE,   1, 
                ,   2, 
            US2E,   1, 
            US3E,   1, 
            Offset (0x0C), 
                ,   1, 
            UADE,   1, 
            UAEE,   1, 
            I23E,   1, 
            SP4E,   1, 
            SD3E,   1, 
                ,   19, 
            XUSE,   1, 
            Offset (0x14), 
                ,   4, 
            KBCE,   1, 
            Offset (0x358), 
            Offset (0x359), 
            SP5E,   1, 
            SP6E,   1, 
                ,   19, 
            HDAE,   1
        }

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

        Device (USB1)
        {
            OperationRegion (USCR, SystemMemory, 0x7D000000, 0x1800)
            Field (USCR, DWordAcc, NoLock, Preserve)
            {
                UNID,   16, 
                Offset (0x154),
                TXPR,   32, 
                Offset (0x174),
                SCPR,   32, 
                Offset (0x1F8),
                UMOD,   32
            }

            Method (_HID, 0, NotSerialized)  // _HID: Hardware ID
            {
                Return ("NVDA0103")
            }

            Name (_CID, "ACPI\\PNP0D20")  // _CID: Compatible ID
            Name (_HRV, 0x02)  // _HRV: Hardware Revision
            Name (_UID, Zero)  // _UID: Unique ID
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Local0 = (0x7D000000 + 0x0100)
                Local1 = (0x1800 - 0x0100)
                Return (UCRS (Local0, Local1, 0x34, 0x03))
            }
        }

        Device (SDM1)
        {
            Method (_HID, 0, NotSerialized)  // _HID: Hardware ID
            {
                Return ("NVDA0212")
            }

            Name (_CID, "PNP0D40" /* SDA Standard Compliant SD Host Controller */)  // _CID: Compatible ID
            Name (_UID, Zero)  // _UID: Unique ID
            Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
            {
                Local0 = GCRS (0x78000000, 0x0200, 0x2E)
                Return (Local0)
            }

            Name (STAF, Zero)
            Method (_STA, 0, NotSerialized)  // _STA: Status
            {
                If ((^STAF == 0xF0))
                {
                    Return (Zero)
                }

                If (^STAF)
                {
                    Return (0x0F)
                }

                If (!^^SD1E)
                {
                    ^STAF = One
                    Return (0x0F)
                }

                Return (Zero)
            }
        }

    } // Scope(_SB)
}
