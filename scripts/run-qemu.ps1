param(
    [string] $Qemu = "qemu-system-x86_64",
    [string] $OvmfCode = "",
    [switch] $Build
)

$ErrorActionPreference = "Stop"
$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$esp = Join-Path $repoRoot "dist\esp"
$bootEfi = Join-Path $esp "EFI\BOOT\BOOTX64.EFI"

if ($Build -or -not (Test-Path $bootEfi)) {
    & (Join-Path $PSScriptRoot "build-standalone-uefi.ps1")
}

if (-not (Test-Path $bootEfi)) {
    throw "BOOTX64.EFI not found after build."
}

$qemuCmd = Get-Command $Qemu -ErrorAction SilentlyContinue
if (-not $qemuCmd -and (Test-Path $Qemu)) {
    $qemuCmd = Get-Item $Qemu
}

if (-not $qemuCmd) {
    throw "qemu-system-x86_64 was not found. Install QEMU or pass -Qemu path."
}

$args = @(
    "-machine", "q35",
    "-m", "256M",
    "-serial", "stdio",
    "-drive", "format=raw,file=fat:rw:$esp"
)

if ($OvmfCode) {
    if (-not (Test-Path $OvmfCode)) {
        throw "OVMF firmware file was not found: $OvmfCode"
    }
    $args = @("-bios", $OvmfCode) + $args
}

& $qemuCmd.Source @args
