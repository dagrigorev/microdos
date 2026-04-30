param(
    [Parameter(Mandatory=$true)] [string] $BootX64Efi,
    [string] $Output = "esp"
)

$ErrorActionPreference = "Stop"
$bootDir = Join-Path $Output "EFI/BOOT"
New-Item -ItemType Directory -Force -Path $bootDir | Out-Null
Copy-Item $BootX64Efi (Join-Path $bootDir "BOOTX64.EFI") -Force
Write-Host "Created EFI System Partition layout at $Output"
