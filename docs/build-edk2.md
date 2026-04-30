# Building MicroDOS with EDK2

This is the primary build path for MicroDOS.

MicroDOS is built as a `UEFI_APPLICATION` module inside an EDK2 workspace. The final firmware binary is copied to:

```text
dist/esp/EFI/BOOT/BOOTX64.EFI
```

That path is the default removable-media boot path for x86_64 UEFI firmware.

## Repository layout

```text
MicroDOS/
  MicroDOSPkg/
    MicroDOSPkg.dec
    MicroDOSPkg.dsc
    MicroDOSApp/
      MicroDOSApp.inf
      Edk2Entry.cpp
      src/...
  src/...                       # host compile-check mirror
  scripts/build-edk2-windows.ps1
  scripts/build-edk2-linux.sh
```

The EDK2 build consumes `MicroDOSPkg/MicroDOSApp/MicroDOSApp.inf`.

## Windows prerequisites

Install:

1. Visual Studio 2022 with C++ workload.
2. Python 3.x.
3. NASM.
4. IASL / ACPICA tools.
5. EDK2 checkout.

Typical EDK2 checkout:

```powershell
git clone https://github.com/tianocore/edk2.git C:\src\edk2
cd C:\src\edk2
git submodule update --init --recursive
```

Build BaseTools once:

```powershell
cd C:\src\edk2
edksetup.bat Rebuild
```

## Windows build command

From the MicroDOS repository root:

```powershell
.\scripts\build-edk2-windows.ps1 -Edk2Root C:\src\edk2 -Target DEBUG -ToolChain VS2022
```

Expected result:

```text
dist\MicroDOSApp.efi
dist\esp\EFI\BOOT\BOOTX64.EFI
```

## Linux build command

From the MicroDOS repository root:

```bash
./scripts/build-edk2-linux.sh ~/src/edk2 DEBUG GCC5
```

Expected result:

```text
dist/MicroDOSApp.efi
dist/esp/EFI/BOOT/BOOTX64.EFI
```

## QEMU boot test

With OVMF installed:

```bash
qemu-system-x86_64 \
  -machine q35 \
  -m 256M \
  -bios /usr/share/OVMF/OVMF_CODE.fd \
  -drive format=raw,file=fat:rw:dist/esp
```

You should see the MicroDOS shell prompt:

```text
A>
```

Try:

```text
HELP
DIR
VER
TYPE README.md
EXIT
```

## VirtualBox test

1. Create a new VM.
2. Enable EFI in VM settings.
3. Attach a FAT-formatted disk/ISO containing:

```text
EFI/BOOT/BOOTX64.EFI
```

4. Boot the VM.

## Current caveat

The current implementation uses a small local UEFI ABI header instead of directly including EDK2 headers. This keeps the core portable and easy to reason about, but some compiler/toolchain combinations may need small ABI macro adjustments. The EDK2 package is now present and is the intended build integration point.
