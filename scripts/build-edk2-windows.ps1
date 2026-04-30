param(
    [string] $Edk2Root = "C:\src\edk2",
    [ValidateSet("DEBUG", "RELEASE")] [string] $Target = "DEBUG",
    [string] $ToolChain = "VS2022",
    [string] $Arch = "X64"
)

$ErrorActionPreference = "Stop"

$env:NASM_PREFIX = ""
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

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe was not found."
}

$vsPath = & $vswhere `
    -version "[17.0,18.0)" `
    -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath

if (-not $vsPath) {
    throw "Visual Studio 2022 with C++ tools was not found."
}

$vcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"

if (-not (Test-Path $vcvars)) {
    throw "vcvars64.bat was not found: $vcvars"
}

$cmdFile = Join-Path $env:TEMP "microdos-edk2-build.cmd"

@"
@echo off
setlocal

set VSCMD_SKIP_SENDTELEMETRY=1
set VSCMD_DEBUG=0
set VSINSTALLDIR=
set VisualStudioVersion=
set NASM_PREFIX=

set PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0\

call "$vcvars"
if errorlevel 1 exit /b %errorlevel%

cd /d "$edk2RootResolved"
if not exist edksetup.bat (
    echo edksetup.bat was not found.
    exit /b 1
)

call edksetup.bat Rebuild
if errorlevel 1 exit /b %errorlevel%

build -p MicroDOSPkg/MicroDOSPkg.dsc -m MicroDOSPkg/MicroDOSApp/MicroDOSApp.inf -a $Arch -t $ToolChain -b $Target
if errorlevel 1 exit /b %errorlevel%

endlocal
exit /b 0
"@ | Set-Content -Path $cmdFile -Encoding ASCII

cmd.exe /d /c "`"$cmdFile`""

if ($LASTEXITCODE -ne 0) {
    throw "EDK2 build failed with exit code $LASTEXITCODE"
}

$efi = Get-ChildItem (Join-Path $edk2RootResolved "Build/MicroDOS") `
    -Recurse `
    -Filter "MicroDOSApp.efi" |
    Select-Object -First 1 -ExpandProperty FullName

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