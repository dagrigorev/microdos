# Architecture

## Why this is a rewrite

Original MS-DOS 1.x was designed around 8086 real mode and IBM PC BIOS. UEFI on modern computers runs in a very different environment: protected/long mode firmware services, PE/COFF applications, no BIOS interrupts, no direct 16-bit process execution.

## Main layers

```text
src/uefi     Firmware-facing platform abstractions
src/core     Freestanding utilities
src/fs       FAT and filesystem compatibility
src/dos      DOS personality and syscall model
src/shell    COMMAND.COM-like shell behavior
```

## Compatibility rule

The public behavior should resemble DOS. The implementation should not preserve unsafe historical internals unless they are required for compatibility.

## Process execution strategy

Native UEFI cannot execute DOS `.COM` programs directly. The planned process layer will emulate enough of an 8086 machine to run simple `.COM` utilities:

- 1 MB address space model
- registers and flags
- PSP at segment base
- command tail
- selected INT 20h and INT 21h services
- file handles mapped to DOS services
