#
#  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.
#  Copyright (c) 2018-2020, Bingxing Wang. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = NintendoSwitch
  PLATFORM_GUID                  = f5e5d453-b715-4e01-8c50-fb298a273fd7
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/NintendoSwitch-$(ARCH)
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = NintendoSwitchPkg/NintendoSwitch.fdf

[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER]
  GCC:*_*_AARCH64_DLINK_FLAGS = -z common-page-size=0x10000

[BuildOptions.common]
  GCC:*_*_AARCH64_CC_FLAGS = -Wno-unused-variable

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag.common]
  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdTurnOffUsbLegacySupport|TRUE

[LibraryClasses.common]
  # ARM Architectural Libraries
  ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  ArmPlatformStackLib|ArmPlatformPkg/Library/ArmPlatformStackLib/ArmPlatformStackLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  TimeBaseLib|EmbeddedPkg/Library/TimeBaseLib/TimeBaseLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  ArmHvcLib|ArmPkg/Library/ArmHvcLib/ArmHvcLib.inf
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  
  # Compiler Services
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  CompilerIntrinsicsLib|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf
  
  # UEFI Platform Services
  AuthVariableLib|MdeModulePkg/Library/AuthVariableLibNull/AuthVariableLibNull.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptDxe/BaseMemoryLibOptDxe.inf
  BmpSupportLib|MdeModulePkg/Library/BaseBmpSupportLib/BaseBmpSupportLib.inf
  BootLogoLib|MdeModulePkg/Library/BootLogoLib/BootLogoLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  CustomizedDisplayLib|MdeModulePkg/Library/CustomizedDisplayLib/CustomizedDisplayLib.inf
  DebugLib|NintendoSwitchPkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  DebugAgentTimerLib|EmbeddedPkg/Library/DebugAgentTimerLibNull/DebugAgentTimerLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  NetLib|NetworkPkg/Library/DxeNetLib/DxeNetLib.inf
  DpcLib|NetworkPkg/Library/DxeDpcLib/DxeDpcLib.inf
  UdpIoLib|NetworkPkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  IpIoLib|NetworkPkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  OrderedCollectionLib|MdePkg/Library/BaseOrderedCollectionRedBlackTreeLib/BaseOrderedCollectionRedBlackTreeLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  TpmMeasurementLib|MdeModulePkg/Library/TpmMeasurementLibNull/TpmMeasurementLibNull.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  VarCheckLib|MdeModulePkg/Library/VarCheckLib/VarCheckLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLib.inf
  VariablePolicyHelperLib|MdeModulePkg/Library/VariablePolicyHelperLib/VariablePolicyHelperLib.inf

  # Board / Platform Libraries
  ArmPlatformLib|NintendoSwitchPkg/Library/NintendoSwitchPlatformLib/NintendoSwitchPlatformLib.inf
  PlatformBootManagerLib|NintendoSwitchPkg/Library/PlatformBootManagerLib/PlatformBootManagerLib.inf

  # Device Libraries
  I2cLib|NintendoSwitchPkg/Library/I2cLib/I2cLib.inf
  GpioLib|NintendoSwitchPkg/Library/GpioLib/GpioLib.inf
  EarlyTimerLib|NintendoSwitchPkg/Library/EarlyTimerLib/EarlyTimerLib.inf
  Max7762xPmicLib|NintendoSwitchPkg/Library/Max7762xPmicLib/Max7762xPmicLib.inf
  UtilLib|NintendoSwitchPkg/Library/UtilLib/UtilLib.inf
  ClockLib|NintendoSwitchPkg/Library/ClockLib/ClockLib.inf

  # 16550 Serial
  # SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  # PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf

  # 8250 Serial
  SerialPortLib|NintendoSwitchPkg/Library/Tegra8250SerialPortLib/Tegra8250SerialPortLib.inf

  # System Libraries
  EfiResetSystemLib|ArmPkg/Library/ArmPsciResetSystemLib/ArmPsciResetSystemLib.inf

  # Support Libraries
  DmaBounceBufferLib|NintendoSwitchPkg/Library/DmaBounceBufferLib/DmaBounceBufferLib.inf
   NonDiscoverableDeviceRegistrationLib|MdeModulePkg/Library/NonDiscoverableDeviceRegistrationLib/NonDiscoverableDeviceRegistrationLib.inf

[LibraryClasses.common.SEC]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PlatformPeiLib|ArmPlatformPkg/PlatformPei/PlatformPeiLib.inf
  ExtractGuidedSectionLib|EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  LzmaDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
  HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PrePiHobListPointerLib|ArmPlatformPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  MemoryAllocationLib|EmbeddedPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf
  MemoryInitPeiLib|NintendoSwitchPkg/Library/MemoryInitPeiLib/MemoryInitPeiLib.inf

[LibraryClasses.common.DXE_CORE]
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  
[LibraryClasses.common.DXE_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  ArmSmcLib|ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  VariablePolicyLib|MdeModulePkg/Library/VariablePolicyLib/VariablePolicyLibRuntimeDxe.inf


################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE
  # Use the Vector Table location in CpuDxe. We will not copy the Vector Table at PcdCpuVectorBaseAddress
  gArmTokenSpaceGuid.PcdRelocateVectorTable|FALSE

[PcdsFixedAtBuild.common]
  gArmPlatformTokenSpaceGuid.PcdCoreCount|4
  gArmTokenSpaceGuid.PcdVFPEnabled|1

  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x2000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxAuthVariableSize|0x2800

  # An assumption here, according to readelf -a bl31.elf
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000
  # Sounds like a pretty safe memory configuration:
  gArmTokenSpaceGuid.PcdSystemMemorySize|0xf0000000

  # Manually set the stack
  # Size of the region used by UEFI in permanent memory (Reserved 128MB)
  gEmbeddedTokenSpaceGuid.PcdPrePiStackSize|0x00040000      # 64K stack
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x40000
  gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize|0x10000000

  ## Default Terminal Type
  ## 0-PCANSI, 1-VT100, 2-VT00+, 3-UTF8, 4-TTYTERM
  gEfiMdePkgTokenSpaceGuid.PcdDefaultTerminalType|4

  gEfiMdeModulePkgTokenSpaceGuid.PcdBootManagerMenuFile|{ 0x21, 0xaa, 0x2c, 0x46, 0x14, 0x76, 0x03, 0x45, 0x83, 0x6e, 0x8a, 0xb6, 0xf4, 0x66, 0x23, 0x31 }
  gEfiMdePkgTokenSpaceGuid.PcdPlatformBootTimeOut|3

  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Nintendo Switch AArch64 UEFI"
  # According to TRM
  # This no longer presents. Just know it is sufficient.
  # gEmbeddedTokenSpaceGuid.PcdPrePiCpuMemorySize|34
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|16

  # ARM platforms usually have fixed memory
  gEfiMdeModulePkgTokenSpaceGuid.PcdResetOnMemoryTypeInformationChange|FALSE
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiReservedMemoryType|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesData|300
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesCode|150
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesCode|1000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesData|12000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderCode|20
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderData|0

  # More ACPI
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"NVIDIA"
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x4f444e45544e494e
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x00000001
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x55424D49
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|0x00000001
  # ACPI predates the AARCH64 architecture by 5 versions, so
  # we only target OSes that support ACPI v5.0 or later
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiExposedTableVersions|0x20

  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|FALSE

  # Debug Configuration
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x800fee0f
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07

  # Interrupt Controller (arch/arm64/boot/dts/nvidia/tegra210.dtsi)
  gEmbeddedTokenSpaceGuid.PcdInterruptBaseAddress|0x50041000
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x50041000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x50042000

  # What the hell is that
  # So Tegra210 has several timer complex?
  gArmTokenSpaceGuid.PcdArmArchTimerSecIntrNum|29
  gArmTokenSpaceGuid.PcdArmArchTimerIntrNum|30
  gArmTokenSpaceGuid.PcdArmArchTimerHypIntrNum|26
  gArmTokenSpaceGuid.PcdArmArchTimerVirtIntrNum|27

  # Core configs
  gArmTokenSpaceGuid.PcdArmPrimaryCore|0
  gArmTokenSpaceGuid.PcdArmPrimaryCoreMask|0xf03
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|19200000

  # Performance
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1

  # Version Info
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Little Moe, LLC."
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"0.01"
  gNintendoSwitchPkgTokenSpaceGuid.PcdSmbiosSystemModel|"Nintendo Switch (HAC-001)"
  gNintendoSwitchPkgTokenSpaceGuid.PcdSmbiosProcessorModel|"NVIDIA Tegra X1"
  gNintendoSwitchPkgTokenSpaceGuid.PcdSmbiosSystemRetailModel|"HAC-001"

  # Display
  # Simple FrameBuffer
  gNintendoSwitchPkgTokenSpaceGuid.PcdMipiFrameBufferAddress|0xdfb80000
  gNintendoSwitchPkgTokenSpaceGuid.PcdMipiFrameBufferWidth|720
  gNintendoSwitchPkgTokenSpaceGuid.PcdMipiFrameBufferHeight|1280
  gNintendoSwitchPkgTokenSpaceGuid.PcdMipiFrameBufferPixelBpp|32

  # TrustZone carveout, 14MB below slot 1 top
  gNintendoSwitchPkgTokenSpaceGuid.PcdTrustZoneCarveoutSize|0xe00000

  #
  # Make VariableRuntimeDxe work at emulated non-volatile variable mode.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdEmuVariableNvModeEnable|TRUE

  # 16550 Serial (UARTB, Right-side Joy-Con)
  # It is sending garbages now, we comment it out right now
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialDetectCable|FALSE
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseMmio|TRUE
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialUseHardwareFlowControl|FALSE
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|115200
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialClockRate|1843000
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0x70006040
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterStride|4
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialLineControl|0x03
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|0x07
  # gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterAccessWidth|8

[Components.common]
  # PEI/SEC
  ArmPlatformPkg/PrePi/PeiUniCore.inf

  # DXE
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
    NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
    NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  }

  # PCD Database
  MdeModulePkg/Universal/PCD/Dxe/Pcd.inf {
    <LibraryClasses>
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  }

  # Runtime Services
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf {
    <LibraryClasses>
	  RealTimeClockLib|NintendoSwitchPkg/Library/VirtualRealTimeClockLib/VirtualRealTimeClockLib.inf
  }

  # Security Stub
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf

  # CPU Services
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf

  # Display
  NintendoSwitchPkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf

  # SMBIOS
  NintendoSwitchPkg/Drivers/SmBiosTableDxe/SmBiosTableDxe.inf

  # ACPI
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf
  NintendoSwitchPkg/AcpiTables/AcpiTables.inf

  # Drivers
  NintendoSwitchPkg/Drivers/SidebandButtonDxe/SidebandButtonDxe.inf
  NintendoSwitchPkg/Drivers/ClockManagementDxe/ClockManagementDxe.inf
  NintendoSwitchPkg/Drivers/PmicDxe/PmicDxe.inf
  NintendoSwitchPkg/Drivers/SdMmcDxe/SdMmcDxe.inf
  NintendoSwitchPkg/Drivers/PinMuxDxe/PinMuxDxe.inf
  # NintendoSwitchPkg/Drivers/EhciPciEmulationDxe/PciEmulation.inf
  MdeModulePkg/Bus/Pci/NonDiscoverablePciDeviceDxe/NonDiscoverablePciDeviceDxe.inf
  MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf {
    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x800fffff
  }
  MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  # Serial
  MdeModulePkg/Universal/SerialDxe/SerialDxe.inf
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf

  # BDS
  MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  MdeModulePkg/Universal/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
  MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  MdeModulePkg/Application/UiApp/UiApp.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootManagerUiLib/BootManagerUiLib.inf
      NULL|MdeModulePkg/Library/BootMaintenanceManagerUiLib/BootMaintenanceManagerUiLib.inf
  }

  # Disk IO
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf

  # Shell
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|8000
  }
