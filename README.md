# EDK2 Implementation for Nintendo Switch (Tegra210)

## Status
Capable to boot something from SD card. If you have a Linux kernel with EFI stub support, supply the device tree for any Tegra210 device (not limited to Nintendo Switch) should boot.

ACPI also boots Windows and Linux, but limited devices are provided (only CPU at this moment).

## Device Support
- CPU services: GIC and Arch Timer.
- Clocks (reset, PLL, etc.)
- Power Management (PMC, PMIC & regulators, etc.)
- GPIO and Pin Multiplexor.
- MicroSD (should support SDSC, HC. XC probed and have partition table shown, but not intensively tested). eMMC support will be added soon.
- Screen and FrameBuffer (need special [Coreboot](https://github.com/imbushuo/Coreboot))
- Side-band buttons, not yet registered as EFI Input Device.

## Planned / In-Progress
- UART
- EHCI USB host
- eMMC
- Sideband buttons as input device
- Joy-Con (maybe not. Need high speed serial)

## Building
See [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg). Almost identical!
