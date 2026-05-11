# MicroDOS VirtualBox workflow

This project no longer uses EDK2 for the normal boot path. The VirtualBox flow is:

1. Build standalone `BOOTX64.EFI` with LLVM/LLD.
2. Generate a FAT16 EFI System Partition image.
3. Convert the raw ESP image to `MicroDOS.vdi` with `VBoxManage convertfromraw`.
4. Create or update a VirtualBox VM with EFI firmware enabled.
5. Attach the generated VDI and boot it.

## Requirements

- LLVM with `clang++` and `lld-link`.
- Oracle VirtualBox with `VBoxManage.exe`.
- PowerShell 7 or Windows PowerShell 5.1.

## Build only

```powershell
.\scripts\build-virtualbox-image.ps1
```

Outputs:

```text
dist\microdos-esp.img
dist\MicroDOS.vdi
```

## Build and run

```powershell
.\scripts\run-virtualbox.ps1 -Recreate
```

Useful options:

```powershell
.\scripts\run-virtualbox.ps1 -Headless
.\scripts\run-virtualbox.ps1 -NoBuild
.\scripts\run-virtualbox.ps1 -Configuration Release
```

## Notes

The generated disk is intentionally tiny and simple: a FAT16 ESP containing only:

```text
EFI\BOOT\BOOTX64.EFI
```

This makes the VM path independent of EDK2, Windows disk mounting, DiskPart, administrator rights, and external FAT image tools.
