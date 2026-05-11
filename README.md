# MicroDOS Standalone Boot

This branch removes the EDK2 dependency from the MicroDOS bring-up path.

The first milestone is intentionally monolithic:

```text
BOOTX64.EFI = MicroBoot loader + MicroDOS kernel
```

The loader is written against a tiny local UEFI ABI header, not EDK2. It initializes GOP framebuffer, captures a memory map, exits boot services, then calls `KernelMain(BootInfo*)`.

## Build on Windows

Requirements:

- LLVM with `clang++.exe` and `lld-link.exe`
- QEMU for fast testing
- OVMF firmware if your QEMU build does not include UEFI firmware

Build:

```powershell
.\scripts\build-standalone-uefi.ps1
```

Clean rebuild:

```powershell
.\scripts\build-standalone-uefi.ps1 -Clean
```

Output:

```text
dist/BOOTX64.EFI
dist/esp/EFI/BOOT/BOOTX64.EFI
```

## Run in QEMU

```powershell
.\scripts\run-qemu.ps1 -Build
```

If QEMU needs explicit OVMF:

```powershell
.\scripts\run-qemu.ps1 -Build -OvmfCode C:\path\to\OVMF_CODE.fd
```

## Host compile-check

```powershell
.\scripts\build-host-check.ps1
```

## Architecture

```text
boot/microboot/efi_min.hpp      minimal UEFI ABI definitions
boot/microboot/main.cpp         standalone UEFI entry, GOP, memory map
kernel/boot/boot_info.hpp       boot protocol between loader and kernel
kernel/kernel.cpp               KernelMain
kernel/platform/framebuffer     framebuffer backend
kernel/platform/console         text console on framebuffer
```

## Why no EDK2

The previous EDK2 path was blocked by unstable environment mixing VS2019, VS2022, VS2026, EDK2 BaseTools, NASM_PREFIX, and C++ type conflicts. This standalone path makes the boot chain deterministic and easier to debug.


## Notes

PowerShell scripts use argument arrays instead of Unix-style `\` line continuations. This keeps them valid in Windows PowerShell and PowerShell 7.
