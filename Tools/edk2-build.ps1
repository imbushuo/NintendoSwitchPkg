#!/usr/bin/pwsh
# Copyright 2018-2019, Bingxing Wang <uefi-oss-projects@imbushuo.net>
# All rights reserved.
#
# This script builds EDK2 content.
# EDK2 setup script should be called before invoking this script.
#

Param
(
	[switch] $Clean,
	[switch] $UseNewerGcc
)

Import-Module $PSScriptRoot/PsModules/redirector.psm1
Import-Module $PSScriptRoot/PsModules/elf.psm1
Write-Host "Task: EDK2 build"

# Targets. Ensure corresponding DSC/FDF files exist
$availableTargets = @(
	"NintendoSwitch"
)

# Check package path.
if ($null -eq (Test-Path -Path "NintendoSwitchPkg")) {
	Write-Error -Message "NintendoSwitchPkg is not found."
	return -2
}

# Probe GCC
# Probe GCC. Use the most suitable one
$ccprefix = Get-GnuAarch64CrossCollectionPath -AllowFallback
if ($null -eq $ccprefix) { return -1 }
if ($false -eq (Test-GnuAarch64CrossCollectionVersionRequirements)) {
	Write-Warning "Failed to check GCC version, build may fail!"
}
$env:GCC5_AARCH64_PREFIX = $ccprefix

Write-Output "Use GCC at $($ccprefix) to run builds."

# Build base tools if not exist (dev).
if (($false -eq (Test-Path -Path "BaseTools")) -or ($Clean -eq $true)) {
	Write-Output "Build base tools."
	make -C BaseTools
	if (-not $?) {
		Write-Error "Base tools target failed."
		return $?
	}
}

if ($true -eq $Clean) {
	foreach ($target in $availableTargets) {
		Write-Output "Clean target $($target)."
		build -a AARCH64 -p NintendoSwitchPkg/$($target).dsc -t GCC5 clean

		if (-not $?) {
			Write-Error "Clean target $($target) failed."
			return $?
		}
	}

	# Apply workaround for "NUL"
	Get-ChildItem -Path Build/**/NUL -Recurse | Remove-Item -Force
}

# Check current commit ID and write it into file for SMBIOS reference. (Trim it)
# Check current date and write it into file for SMBIOS reference too. (MM/dd/yyyy)

Write-Output "Stamp build."
$commit = git rev-parse HEAD
$date = (Get-Date).Date.ToString("MM/dd/yyyy")
if ($commit) {
	$commit = $commit.Substring(0, 8)

	$releaseInfoContent = @(
		"#ifndef __SMBIOS_RELEASE_INFO_H__",
		"#define __SMBIOS_RELEASE_INFO_H__",
		"#ifdef __IMPL_COMMIT_ID__",
		"#undef __IMPL_COMMIT_ID__",
		"#endif",
		"#define __IMPL_COMMIT_ID__ `"$($commit)`"",
		"#ifdef __RELEASE_DATE__",
		"#undef __RELEASE_DATE__",
		"#endif",
		"#define __RELEASE_DATE__ `"$($date)`"",
		"#endif"
	)

	Set-Content -Path NintendoSwitchPkg/Include/FwReleaseInfo.h -Value $releaseInfoContent -ErrorAction SilentlyContinue -Force
}

foreach ($target in $availableTargets) {
	Write-Output "Build NintendoSwitchPkg for $($target) (DEBUG)."
	build -a AARCH64 -p NintendoSwitchPkg/$($target).dsc -t GCC5

	if (-not $?) {
		Write-Error "Build target $($target) failed."
		return $?
	}
}

# Invoke ELF build.
Copy-ElfImages
