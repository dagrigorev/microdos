param(
    [string] $BootEfiPath = "",
    [string] $ImagePath = "",
    [int] $SizeMiB = 64,
    [string] $VolumeLabel = "MICRODOS"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$dist = Join-Path $repoRoot "dist"
$espDir = Join-Path $dist "esp"
$defaultBootEfi = Join-Path $espDir "EFI\BOOT\BOOTX64.EFI"

if (-not $BootEfiPath) {
    $BootEfiPath = $defaultBootEfi
}

if (-not $ImagePath) {
    $ImagePath = Join-Path $dist "microdos-esp.img"
}

if (-not (Test-Path $BootEfiPath)) {
    throw "BOOTX64.EFI not found: $BootEfiPath. Run scripts/build-standalone-uefi.ps1 first."
}

if ($SizeMiB -lt 8) {
    throw "SizeMiB must be at least 8."
}

New-Item -ItemType Directory -Force -Path $dist | Out-Null

$bytesPerSector = 512
$sectorsPerCluster = 4
$reservedSectors = 1
$fatCount = 2
$rootEntryCount = 512
$mediaDescriptor = 0xF8
$totalSectors = [int](($SizeMiB * 1024 * 1024) / $bytesPerSector)
$rootDirSectors = [int](($rootEntryCount * 32 + ($bytesPerSector - 1)) / $bytesPerSector)
$fatSectors = 128
$dataSectors = $totalSectors - $reservedSectors - ($fatCount * $fatSectors) - $rootDirSectors
$clusterCount = [int]($dataSectors / $sectorsPerCluster)

if ($clusterCount -lt 4085) {
    throw "Image is too small for FAT16 with current layout. Increase SizeMiB."
}

if ($clusterCount -ge 65525) {
    throw "Image is too large for FAT16 with current layout. Decrease SizeMiB."
}

$rootDirLba = $reservedSectors + ($fatCount * $fatSectors)
$dataStartLba = $rootDirLba + $rootDirSectors

function Write-Le16([byte[]] $Buffer, [int] $Offset, [int] $Value) {
    $Buffer[$Offset] = [byte]($Value -band 0xFF)
    $Buffer[$Offset + 1] = [byte](($Value -shr 8) -band 0xFF)
}

function Write-Le32([byte[]] $Buffer, [int] $Offset, [UInt32] $Value) {
    $Buffer[$Offset] = [byte]($Value -band 0xFF)
    $Buffer[$Offset + 1] = [byte](($Value -shr 8) -band 0xFF)
    $Buffer[$Offset + 2] = [byte](($Value -shr 16) -band 0xFF)
    $Buffer[$Offset + 3] = [byte](($Value -shr 24) -band 0xFF)
}

function Get-ShortNameBytes([string] $Name8, [string] $Ext3 = "") {
    $name = ($Name8.ToUpperInvariant()).PadRight(8).Substring(0, 8)
    $ext = ($Ext3.ToUpperInvariant()).PadRight(3).Substring(0, 3)
    return [System.Text.Encoding]::ASCII.GetBytes($name + $ext)
}

function New-DirEntry([string] $Name8, [string] $Ext3, [byte] $Attributes, [int] $FirstCluster, [UInt32] $Size) {
    $entry = New-Object byte[] 32
    $name = Get-ShortNameBytes $Name8 $Ext3
    [Array]::Copy($name, 0, $entry, 0, 11)
    $entry[11] = $Attributes
    Write-Le16 $entry 26 $FirstCluster
    Write-Le32 $entry 28 $Size
    return $entry
}

function Set-Fat16Entry([byte[]] $Fat, [int] $Cluster, [int] $Value) {
    $offset = $Cluster * 2
    $Fat[$offset] = [byte]($Value -band 0xFF)
    $Fat[$offset + 1] = [byte](($Value -shr 8) -band 0xFF)
}

function Get-ClusterOffset([int] $Cluster) {
    return (($dataStartLba + (($Cluster - 2) * $sectorsPerCluster)) * $bytesPerSector)
}

$imageSize = $totalSectors * $bytesPerSector
$image = New-Object byte[] $imageSize

# BIOS Parameter Block / FAT16 boot sector. This is a data ESP image, not executable boot code.
$boot = New-Object byte[] $bytesPerSector
$boot[0] = 0xEB; $boot[1] = 0x3C; $boot[2] = 0x90
[Array]::Copy([System.Text.Encoding]::ASCII.GetBytes("MICRODOS"), 0, $boot, 3, 8)
Write-Le16 $boot 11 $bytesPerSector
$boot[13] = [byte]$sectorsPerCluster
Write-Le16 $boot 14 $reservedSectors
$boot[16] = [byte]$fatCount
Write-Le16 $boot 17 $rootEntryCount
if ($totalSectors -lt 65536) {
    Write-Le16 $boot 19 $totalSectors
    Write-Le32 $boot 32 0
} else {
    Write-Le16 $boot 19 0
    Write-Le32 $boot 32 ([UInt32]$totalSectors)
}
$boot[21] = [byte]$mediaDescriptor
Write-Le16 $boot 22 $fatSectors
Write-Le16 $boot 24 63
Write-Le16 $boot 26 255
Write-Le32 $boot 28 0
$boot[36] = 0x80
$boot[38] = 0x29
Write-Le32 $boot 39 0x4D44534F
$label = ($VolumeLabel.ToUpperInvariant()).PadRight(11).Substring(0, 11)
[Array]::Copy([System.Text.Encoding]::ASCII.GetBytes($label), 0, $boot, 43, 11)
[Array]::Copy([System.Text.Encoding]::ASCII.GetBytes("FAT16   "), 0, $boot, 54, 8)
$boot[510] = 0x55; $boot[511] = 0xAA
[Array]::Copy($boot, 0, $image, 0, $bytesPerSector)

$bootBytes = [System.IO.File]::ReadAllBytes((Resolve-Path $BootEfiPath))
$clustersForFile = [int][Math]::Ceiling($bootBytes.Length / ($sectorsPerCluster * $bytesPerSector))
if ($clustersForFile -lt 1) { $clustersForFile = 1 }

$efiCluster = 2
$bootDirCluster = 3
$fileStartCluster = 4
$fileEndCluster = $fileStartCluster + $clustersForFile - 1
if ($fileEndCluster -ge $clusterCount) {
    throw "BOOTX64.EFI is too large for the generated FAT image. Increase SizeMiB."
}

$fat = New-Object byte[] ($fatSectors * $bytesPerSector)
$fat[0] = [byte]$mediaDescriptor
$fat[1] = 0xFF
$fat[2] = 0xFF
$fat[3] = 0xFF
Set-Fat16Entry $fat $efiCluster 0xFFFF
Set-Fat16Entry $fat $bootDirCluster 0xFFFF
for ($c = $fileStartCluster; $c -le $fileEndCluster; $c++) {
    if ($c -eq $fileEndCluster) {
        Set-Fat16Entry $fat $c 0xFFFF
    } else {
        Set-Fat16Entry $fat $c ($c + 1)
    }
}

for ($fatIndex = 0; $fatIndex -lt $fatCount; $fatIndex++) {
    $fatOffset = ($reservedSectors + ($fatIndex * $fatSectors)) * $bytesPerSector
    [Array]::Copy($fat, 0, $image, $fatOffset, $fat.Length)
}

# Root directory: /EFI
$rootOffset = $rootDirLba * $bytesPerSector
$efiEntry = New-DirEntry "EFI" "" 0x10 $efiCluster 0
[Array]::Copy($efiEntry, 0, $image, $rootOffset, 32)

# /EFI directory: . .. BOOT
$efiDirOffset = Get-ClusterOffset $efiCluster
$dot = New-DirEntry "." "" 0x10 $efiCluster 0
$dotdot = New-DirEntry ".." "" 0x10 0 0
$bootDirEntry = New-DirEntry "BOOT" "" 0x10 $bootDirCluster 0
[Array]::Copy($dot, 0, $image, $efiDirOffset, 32)
[Array]::Copy($dotdot, 0, $image, $efiDirOffset + 32, 32)
[Array]::Copy($bootDirEntry, 0, $image, $efiDirOffset + 64, 32)

# /EFI/BOOT directory: . .. BOOTX64.EFI
$bootDirOffset = Get-ClusterOffset $bootDirCluster
$dotBoot = New-DirEntry "." "" 0x10 $bootDirCluster 0
$dotdotBoot = New-DirEntry ".." "" 0x10 $efiCluster 0
$bootFileEntry = New-DirEntry "BOOTX64" "EFI" 0x20 $fileStartCluster ([UInt32]$bootBytes.Length)
[Array]::Copy($dotBoot, 0, $image, $bootDirOffset, 32)
[Array]::Copy($dotdotBoot, 0, $image, $bootDirOffset + 32, 32)
[Array]::Copy($bootFileEntry, 0, $image, $bootDirOffset + 64, 32)

# File data
$fileOffset = Get-ClusterOffset $fileStartCluster
[Array]::Copy($bootBytes, 0, $image, $fileOffset, $bootBytes.Length)

[System.IO.File]::WriteAllBytes($ImagePath, $image)

Write-Host "Done."
Write-Host "ESP dir   : $espDir"
Write-Host "Raw image : $ImagePath"
Write-Host "Size      : $SizeMiB MiB"
Write-Host "Contains  : EFI\BOOT\BOOTX64.EFI"
