param(
    [ValidateSet("Debug", "Release", "DEBUG", "RELEASE")]
    [string] $Configuration = "Debug",

    [string] $Arch = "x64",

    [switch] $Clean
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildRoot = Join-Path $repoRoot "build"
$outRoot = Join-Path $buildRoot "standalone-uefi"
$objRoot = Join-Path $outRoot "obj"
$distRoot = Join-Path $repoRoot "dist"
$espBoot = Join-Path $distRoot "esp\EFI\BOOT"
$efiOut = Join-Path $espBoot "BOOTX64.EFI"
$appOut = Join-Path $distRoot "MicroDOSApp.efi"

$configNormalized = if ($Configuration.ToLowerInvariant() -eq "release") { "Release" } else { "Debug" }

Write-Host "MicroDOS standalone UEFI build"
Write-Host "Repository   : $repoRoot"
Write-Host "Configuration: $configNormalized"
Write-Host "Arch         : $Arch"

if ($Clean) {
    Remove-Item $outRoot -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item $espBoot -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item $appOut -Force -ErrorAction SilentlyContinue
}

New-Item -ItemType Directory -Force -Path $objRoot | Out-Null
New-Item -ItemType Directory -Force -Path $espBoot | Out-Null
New-Item -ItemType Directory -Force -Path $distRoot | Out-Null

function Find-Tool {
    param(
        [string[]] $Names,
        [string[]] $ExtraPaths = @()
    )

    foreach ($path in $ExtraPaths) {
        foreach ($name in $Names) {
            $candidate = Join-Path $path $name
            if (Test-Path $candidate) {
                return $candidate
            }
        }
    }

    foreach ($name in $Names) {
        $cmd = Get-Command $name -ErrorAction SilentlyContinue
        if ($cmd) {
            return $cmd.Source
        }
    }

    return $null
}

$llvmPaths = @(
    "C:\Program Files\LLVM\bin",
    "$env:USERPROFILE\scoop\apps\llvm\current\bin",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\Llvm\x64\bin",
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin"
)

$clang = Find-Tool -Names @("clang++.exe", "clang++.cmd", "clang++") -ExtraPaths $llvmPaths
$lldLink = Find-Tool -Names @("lld-link.exe", "lld-link") -ExtraPaths $llvmPaths

if (-not $clang) {
    throw "clang++ was not found. Install LLVM or add clang++ to PATH."
}

if (-not $lldLink) {
    throw "lld-link was not found. Install LLVM or add lld-link to PATH."
}

Write-Host "clang++     : $clang"
Write-Host "lld-link    : $lldLink"

$sources = @(
    "boot/standalone_uefi/efi_main.cpp",
    "kernel/kernel_main.cpp",
    "kernel/platform/console/framebuffer_console.cpp",
    "kernel/platform/console/serial_console.cpp",
    "kernel/platform/framebuffer/framebuffer.cpp"
)

$commonArgs = @(
    "-target", "x86_64-pc-win32-coff",
    "-std=c++20",
    "-ffreestanding",
    "-fno-exceptions",
    "-fno-rtti",
    "-fno-stack-protector",
    "-fshort-wchar",
    "-mno-red-zone",
    "-Wall",
    "-Wextra",
    "-Wno-unused-parameter",
    "-I", "$repoRoot",
    "-I", (Join-Path $repoRoot "boot"),
    "-I", (Join-Path $repoRoot "kernel"),
    "-D", "MICRODOS_STANDALONE_UEFI=1"
)

if ($configNormalized -eq "Release") {
    $commonArgs += @("-O2", "-DNDEBUG")
} else {
    $commonArgs += @("-O0", "-g", "-DDEBUG")
}

$objFiles = @()

foreach ($source in $sources) {
    $sourcePath = Join-Path $repoRoot $source
    if (-not (Test-Path $sourcePath)) {
        Write-Warning "Source file not found, skipping: $source"
        continue
    }

    $objName = ($source -replace "[\\/:]", "_") -replace "\.cpp$", ".obj"
    $objPath = Join-Path $objRoot $objName

    Write-Host "Compiling $source"
    & $clang @commonArgs -c $sourcePath -o $objPath

    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed: $source"
    }

    $objFiles += $objPath
}

if ($objFiles.Count -eq 0) {
    throw "No object files were produced. Check source paths in build-standalone-uefi.ps1."
}

$linkArgs = @(
    "/entry:efi_main",
    "/subsystem:efi_application",
    "/machine:x64",
    "/nodefaultlib",
    "/debug",
    "/out:$efiOut"
)

$linkArgs += $objFiles

Write-Host "Linking $efiOut"
& $lldLink @linkArgs

if ($LASTEXITCODE -ne 0) {
    throw "Linking failed."
}

Copy-Item $efiOut $appOut -Force

Write-Host "Done."
Write-Host "Boot EFI: $efiOut"
Write-Host "EFI app : $appOut"
