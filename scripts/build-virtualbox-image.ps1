param(
    [ValidateSet("Debug", "Release", "DEBUG", "RELEASE")]
    [string] $Configuration = "Debug",

    [int] $ImageSizeMb = 64,

    [switch] $Clean
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$dist = Join-Path $repoRoot "dist"
$espDir = Join-Path $dist "esp"
$imageDir = Join-Path $dist "images"
$rawImage = Join-Path $imageDir "MicroDOS.raw"
$vdiImage = Join-Path $imageDir "MicroDOS.vdi"

Write-Host "MicroDOS VirtualBox image builder"
Write-Host "Repository   : $repoRoot"
Write-Host "Configuration: $Configuration"
Write-Host "Image size   : $ImageSizeMb MB"
Write-Host "Clean        : $Clean"

if ($Clean) {
    Remove-Item $imageDir -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item $espDir -Recurse -Force -ErrorAction SilentlyContinue
}

New-Item -ItemType Directory -Force -Path $imageDir | Out-Null

$buildScript = Join-Path $PSScriptRoot "build-standalone-uefi.ps1"
if (-not (Test-Path $buildScript)) {
    throw "build-standalone-uefi.ps1 was not found: $buildScript"
}

$buildArgs = @{
    Configuration = $Configuration
}

& $buildScript @buildArgs

if ($LASTEXITCODE -ne 0) {
    throw "Standalone UEFI build failed with exit code $LASTEXITCODE"
}

$bootEfi = Join-Path $espDir "EFI\BOOT\BOOTX64.EFI"
if (-not (Test-Path $bootEfi)) {
    throw "BOOTX64.EFI was not found after build: $bootEfi"
}

$createEspScript = Join-Path $PSScriptRoot "create-fat-esp.ps1"
if (Test-Path $createEspScript) {
    $espArgs = @{
        ImagePath = $rawImage
        SourceDirectory = $espDir
        SizeMb = $ImageSizeMb
        Clean = $Clean
    }

    & $createEspScript @espArgs

    if ($LASTEXITCODE -ne 0) {
        throw "ESP image creation failed with exit code $LASTEXITCODE"
    }
}
else {
    Write-Warning "create-fat-esp.ps1 was not found. Creating a zero-filled raw placeholder image."

    $bytes = [int64]$ImageSizeMb * 1024 * 1024
    $fs = [System.IO.File]::Open($rawImage, [System.IO.FileMode]::Create, [System.IO.FileAccess]::ReadWrite)
    try {
        $fs.SetLength($bytes)
    }
    finally {
        $fs.Dispose()
    }
}

if (-not (Test-Path $rawImage)) {
    throw "Raw image was not created: $rawImage"
}

$virtualBoxCandidates = @(
    "$env:ProgramFiles\Oracle\VirtualBox\VBoxManage.exe",
    "${env:ProgramFiles(x86)}\Oracle\VirtualBox\VBoxManage.exe"
) | Where-Object { $_ -and (Test-Path $_) }

$vboxManage = $virtualBoxCandidates | Select-Object -First 1

if ($vboxManage) {
    if (Test-Path $vdiImage) {
        Remove-Item $vdiImage -Force
    }

    & $vboxManage convertfromraw $rawImage $vdiImage --format VDI

    if ($LASTEXITCODE -ne 0) {
        throw "VBoxManage convertfromraw failed with exit code $LASTEXITCODE"
    }
}
else {
    Write-Warning "VBoxManage.exe was not found. Raw image was created, but VDI conversion was skipped."
}

Write-Host "Done."
Write-Host "ESP dir  : $espDir"
Write-Host "Raw image: $rawImage"
if (Test-Path $vdiImage) {
    Write-Host "VDI image: $vdiImage"
}
