# MicroDOS

MicroDOS is a modern C++ UEFI-native DOS-compatible micro-OS foundation inspired by the uploaded MS-DOS 1.25 source tree.

It is **not** a direct binary port of MS-DOS. MS-DOS 1.x depends on 16-bit real mode, BIOS interrupts, segment registers, IVT, PSP layout, FCB APIs, and direct `.COM`/`.EXE` loading assumptions that do not exist in UEFI x86_64.

The correct design is a compatibility rewrite:

```text
UEFI Platform Layer
    -> DOS Kernel Services
        -> FAT / File Abstraction
            -> Command Shell
                -> Optional 8086 .COM Emulator
```

## Current stage

Implemented foundation:

- EDK2 package: `MicroDOSPkg`.
- UEFI application module: `MicroDOSApp.inf`.
- `UefiMain` bridge to the C++ MicroDOS entry point.
- Minimal UEFI type/protocol declarations.
- Console abstraction over UEFI text output/input.
- UEFI LoadedImageProtocol + SimpleFileSystemProtocol binding.
- Interactive command shell.
- Implemented commands: `HELP`, `CLS`, `ECHO`, `VER`, `MEM`, `EXIT`, `REM`, `DIR`, `TYPE`.
- Recognized from `COMMAND.ASM`: `COPY`, `DEL`/`ERASE`, `REN`/`RENAME`, `PAUSE`, `DATE`, `TIME`.
- DOS service boundary stubs.
- FAT BPB parser foundation.
- Source analysis docs for the uploaded MS-DOS 1.25 archive.

## Build with EDK2

Windows:

```powershell
.\scripts\build-edk2-windows.ps1 -Edk2Root C:\src\edk2 -Target DEBUG -ToolChain VS2022
```

Linux:

```bash
./scripts/build-edk2-linux.sh ~/src/edk2 DEBUG GCC5
```

Expected output:

```text
dist/esp/EFI/BOOT/BOOTX64.EFI
```

See `docs/build-edk2.md` for full setup and QEMU/VirtualBox boot notes.

## Source-guided compatibility target

From `COMMAND.ASM`, the DOS 1.x internal command set is:

```text
DIR, RENAME, REN, ERASE, DEL, TYPE, REM, COPY, PAUSE, DATE, TIME
```

From `MSDOS.ASM`, the filesystem compatibility target is FAT12-style 8.3 directory handling with 32-byte directory entries and packed 12-bit FAT chains.
