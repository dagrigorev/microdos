param(
    [ValidateSet("Debug", "Release", "DEBUG", "RELEASE")]
    [string] $Configuration = "Debug",

    [int] $ImageSizeMb = 64,

    [string] $VmName = "MicroDOS",

    [switch] $Recreate,

    [switch] $NoStart
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$distDir = Join-Path $repoRoot "dist"
$vmDir = Join-Path $distDir "virtualbox"
$vhdPath = Join-Path $vmDir "MicroDOS.vhd"

Write-Host "MicroDOS VirtualBox runner"
Write-Host "Repository   : $repoRoot"
Write-Host "Configuration: $Configuration"
Write-Host "Image size   : $ImageSizeMb MB"
Write-Host "VM name      : $VmName"
Write-Host "Recreate     : $Recreate"

function Find-VBoxManage {
    $candidates = @(
        (Join-Path ${env:ProgramFiles} "Oracle\VirtualBox\VBoxManage.exe"),
        (Join-Path ${env:ProgramFiles(x86)} "Oracle\VirtualBox\VBoxManage.exe"),
        "VBoxManage.exe"
    )

    foreach ($candidate in $candidates) {
        try {
            $command = Get-Command $candidate -ErrorAction Stop
            return $command.Source
        }
        catch {
            if (Test-Path $candidate) {
                return $candidate
            }
        }
    }

    throw "VBoxManage.exe was not found. Install Oracle VirtualBox or add VBoxManage.exe to PATH."
}

$vboxManage = Find-VBoxManage
Write-Host "VBoxManage   : $vboxManage"

$buildScript = Join-Path $PSScriptRoot "build-virtualbox-image.ps1"
if (-not (Test-Path $buildScript)) {
    throw "Build script was not found: $buildScript"
}

$buildArgs = @{
    Configuration = $Configuration
    ImageSizeMb   = $ImageSizeMb
    Clean         = [bool]$Recreate
}

& $buildScript @buildArgs

if (-not (Test-Path $vhdPath)) {
    throw "VirtualBox disk image was not found: $vhdPath"
}

function Test-VBoxVmExists {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Name
    )

    & $vboxManage showvminfo $Name *> $null
    return ($LASTEXITCODE -eq 0)
}

function Stop-VBoxVmIfRunning {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Name
    )

    $info = & $vboxManage showvminfo $Name --machinereadable 2>$null
    if ($LASTEXITCODE -ne 0) {
        return
    }

    $stateLine = $info | Where-Object { $_ -like "VMState=*" } | Select-Object -First 1
    if ($stateLine -match '"running"') {
        Write-Host "Stopping running VM: $Name"
        & $vboxManage controlvm $Name poweroff | Out-Null
        Start-Sleep -Seconds 2
    }
}

function Remove-VBoxVmIfExists {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Name
    )

    if (Test-VBoxVmExists -Name $Name) {
        Stop-VBoxVmIfRunning -Name $Name

        Write-Host "Removing existing VM: $Name"
        & $vboxManage unregistervm $Name --delete | Out-Null

        if ($LASTEXITCODE -ne 0) {
            throw "Failed to remove existing VM: $Name"
        }
    }
}

if ($Recreate) {
    Remove-VBoxVmIfExists -Name $VmName
}

if (-not (Test-VBoxVmExists -Name $VmName)) {
    Write-Host "Creating VM: $VmName"

    New-Item -ItemType Directory -Force -Path $vmDir | Out-Null

    & $vboxManage createvm `
        --name $VmName `
        --ostype "Other_64" `
        --basefolder $vmDir `
        --register | Out-Null

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to create VirtualBox VM: $VmName"
    }

    & $vboxManage modifyvm $VmName `
        --firmware efi `
        --memory 256 `
        --vram 32 `
        --cpus 1 `
        --ioapic on `
        --rtcuseutc on `
        --boot1 disk `
        --boot2 none `
        --boot3 none `
        --boot4 none `
        --audio none `
        --usb off `
        --pae on | Out-Null

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to configure VirtualBox VM: $VmName"
    }

    & $vboxManage storagectl $VmName `
        --name "SATA" `
        --add sata `
        --controller IntelAhci `
        --portcount 1 `
        --bootable on | Out-Null

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to create SATA controller for VM: $VmName"
    }
}
else {
    Write-Host "VM already exists: $VmName"
    Stop-VBoxVmIfRunning -Name $VmName
}

# Detach any previous disk from SATA port 0.
& $vboxManage storageattach $VmName `
    --storagectl "SATA" `
    --port 0 `
    --device 0 `
    --medium none 2>$null | Out-Null

# Remove stale registered medium entry if VirtualBox still remembers it.
& $vboxManage closemedium disk $vhdPath --delete 2>$null | Out-Null

# The previous command may delete the freshly created VHD if it was registered.
# Rebuild it again when Recreate is used and the VHD disappeared.
if (-not (Test-Path $vhdPath)) {
    & $buildScript @buildArgs

    if (-not (Test-Path $vhdPath)) {
        throw "VirtualBox disk image was not found after rebuild: $vhdPath"
    }
}

Write-Host "Attaching disk: $vhdPath"

& $vboxManage storageattach $VmName `
    --storagectl "SATA" `
    --port 0 `
    --device 0 `
    --type hdd `
    --medium $vhdPath | Out-Null

if ($LASTEXITCODE -ne 0) {
    throw "Failed to attach disk image to VM: $vhdPath"
}

Write-Host "Done."
Write-Host "VM       : $VmName"
Write-Host "Disk     : $vhdPath"

if (-not $NoStart) {
    Write-Host "Starting VM..."
    & $vboxManage startvm $VmName
}
else {
    Write-Host "VM was prepared but not started because -NoStart was specified."
}
