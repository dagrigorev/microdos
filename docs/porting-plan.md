# 10-Step Porting Plan

## 1. Inventory original DOS components

Separate boot code, DOS kernel services, COMMAND.COM behavior, device abstraction, FAT access, process loading, and BIOS interrupt usage.

## 2. Establish UEFI-native target

Use x86_64 UEFI application first. Avoid BIOS, real mode, and native execution of 16-bit code.

## 3. Implement UEFI platform layer

Wrap console, memory, loaded image, filesystem, and block I/O protocols behind stable C++ interfaces.

## 4. Build the command shell first

A shell gives immediate validation for input, output, command parsing, filesystem, and service behavior.

## 5. Replace BIOS disk and screen calls

Map BIOS disk calls to UEFI SimpleFileSystem / BlockIo and BIOS text calls to SimpleTextOutput or GOP framebuffer text rendering.

## 6. Recreate DOS kernel services

Model DOS INT 21h functions as internal C++ service calls instead of preserving the old interrupt implementation.

## 7. Implement FAT compatibility

Start with FAT12/FAT16 and 8.3 filenames. Add FAT32 and long filenames later as optional extensions.

## 8. Rebuild COMMAND.COM behavior

Implement commands as native C++ shell commands, then add batch support.

## 9. Add DOS process compatibility

UEFI cannot natively execute 16-bit COM/EXE files. Implement an 8086 interpreter with PSP, registers, memory arena, and selected INT 20h/21h services.

## 10. Harden and modernize

Add tests, disk image tests, QEMU/VirtualBox boot scripts, UTF-8 support, framebuffer UI, and modern commands without breaking DOS semantics.
