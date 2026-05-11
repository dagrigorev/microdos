# MicroDOS no-EDK2 port

## Removed dependencies

- EDK2 packages
- `.dsc`, `.dec`, `.inf`
- `UefiLib`, `BaseLib`, `PcdLib`, `StackCheckLib`
- EDK2 `CHAR16` conflict surface
- EDK2 BaseTools build chain

## Current boot model

`BOOTX64.EFI` is produced directly by LLVM/LLD as a PE/COFF EFI application.

It currently contains both:

- MicroBoot loader
- MicroDOS kernel

This is deliberate for the first stable milestone. Once framebuffer console and boot protocol are stable, the loader can be split to load `MICRODOS.KRN` from the ESP.

## Boot protocol

The loader passes `microdos::boot::BootInfo` to the kernel.

Important fields:

- framebuffer base
- resolution
- pixels per scanline
- memory map address and descriptor size
- optional initrd information

## Output system

The kernel no longer writes through firmware console after `ExitBootServices`.

Output is now:

```text
Framebuffer -> TextConsole
```

Serial output can be added next as `kernel/platform/console/serial_console.*`.

## Next steps

1. Add COM1 serial debug console.
2. Add keyboard input through a pre-exit boot service stage or PS/2/USB plan.
3. Split kernel into a separate `MICRODOS.KRN` binary.
4. Add PE/COFF or flat binary loader.
5. Add page table ownership after `ExitBootServices`.
6. Add memory manager using the captured memory map.
