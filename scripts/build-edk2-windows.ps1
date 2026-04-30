param(
    [Parameter(Mandatory=$true)] [string] $Edk2Root,
    [ValidateSet("DEBUG", "RELEASE")] [string] $Target = "DEBUG",
    [string] $ToolChain = "VS2022",
    [string] $Arch = "X64"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$edk2RootResolved = Resolve-Path $Edk2Root
$packageTarget = Join-Path $edk2RootResolved "MicroDOSPkg"

Write-Host "MicroDOS EDK2 build"
Write-Host "Repository: $repoRoot"
Write-Host "EDK2 root : $edk2RootResolved"
Write-Host "Target    : $Target"
Write-Host "Toolchain : $ToolChain"
Write-Host "Arch      : $Arch"

if (Test-Path $packageTarget) {
    Remove-Item $packageTarget -Recurse -Force
}
Copy-Item (Join-Path $repoRoot "MicroDOSPkg") $packageTarget -Recurse -Force

Push-Location $edk2RootResolved
try {
    if (-not (Test-Path "edksetup.bat")) {
        throw "edksetup.bat was not found. The path does not look like an EDK2 checkout."
    }

    cmd /c "edksetup.bat Rebuild && build -p MicroDOSPkg/MicroDOSPkg.dsc -m MicroDOSPkg/MicroDOSApp/MicroDOSApp.inf -a $Arch -t $ToolChain -b $Target"
    if ($LASTEXITCODE -ne 0) {
        throw "EDK2 build failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}

$efi = Get-ChildItem (Join-Path $edk2RootResolved "Build/MicroDOS") -Recurse -Filter "MicroDOSApp.efi" | Select-Object -First 1 -ExpandProperty FullName
if (-not $efi -or -not (Test-Path $efi)) {
    throw "Build finished, but MicroDOSApp.efi was not found under Build/MicroDOS."
}

$dist = Join-Path $repoRoot "dist"
$espBoot = Join-Path $dist "esp/EFI/BOOT"
New-Item -ItemType Directory -Force -Path $espBoot | Out-Null
Copy-Item $efi (Join-Path $espBoot "BOOTX64.EFI") -Force
Copy-Item $efi (Join-Path $dist "MicroDOSApp.efi") -Force

Write-Host "Done."
Write-Host "EFI app : $efi"
Write-Host "ESP     : $(Join-Path $dist 'esp')"
Write-Host "Boot EFI: $(Join-Path $espBoot 'BOOTX64.EFI')"
