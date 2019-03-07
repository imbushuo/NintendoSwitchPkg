# EDK2 Implementation for Nintendo Switch (Tegra210)

## Status
Not quite useful right now. Capable to boot something from SD card.

> Currently fighting ACPI. ACPI is winning (as usual). - [@zhuowei](https://twitter.com/zhuowei/status/1100207058576494592)

## Device Support
- CPU services: GIC and Arch Timer.
- Clocks (reset, PLL, etc.)
- MicroSD (should support SDSC, HC. XC probed and have partition table shown, but not intensively tested). eMMC support will be added soon.
- Screen and FrameBuffer (need special [Coreboot](https://github.com/imbushuo/Coreboot))
- Side-band buttons, not yet registered as EFI Input Device.

## Building
See [Lumia950XLPkg](https://github.com/WOA-Project/Lumia950XLPkg). Almost identical!
