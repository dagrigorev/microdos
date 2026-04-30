# MicroDOS

A modern C++ foundation for a UEFI-native DOS-compatible micro-OS inspired by MS-DOS 1.x architecture.

This is not a direct binary port of MS-DOS. MS-DOS 1.x depends on 16-bit real mode, BIOS interrupts, segment registers, IVT, PSP layout, and direct assumptions that do not exist in UEFI x86_64. The correct path is a compatibility rewrite:

```text
UEFI Platform Layer
    -> DOS Kernel Services
        -> FAT / File Abstraction
            -> Command Shell
                -> Optional 8086 .COM Emulator
```

## Current stage

Implemented foundation:

- Minimal UEFI type/protocol declarations.
- `efi_main` entry point.
- Console abstraction over UEFI text output/input.
- Interactive command shell.
- Built-in commands: `HELP`, `CLS`, `ECHO`, `VER`, `MEM`, `EXIT`.
- DOS service boundary stubs.
- FAT and process model design placeholders with concrete interfaces.

## Intended next stages

1. Add Simple File System protocol binding.
2. Implement `DIR`, `TYPE`, `CD`.
3. Add FAT12/FAT16 volume parser for DOS disk images.
4. Add DOS syscall compatibility table for selected INT 21h services.
5. Add a tiny 8086 `.COM` interpreter.

## Build options

This repository is intentionally dependency-light. It can be integrated into:

- EDK2 as an application module.
- GNU-EFI based build.
- LLVM/LLD PE/COFF UEFI build.

See `docs/build.md`.
