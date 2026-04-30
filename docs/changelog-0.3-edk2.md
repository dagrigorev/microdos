# MicroDOS 0.3 EDK2 Integration

## Added

- Renamed project to `MicroDOS` across root docs, CMake target names, namespace, and shell banner.
- Added `MicroDOSPkg` EDK2 package.
- Added `MicroDOSPkg.dec`.
- Added `MicroDOSPkg.dsc`.
- Added `MicroDOSApp.inf` as a `UEFI_APPLICATION` module.
- Added `Edk2Entry.cpp` bridge exposing `UefiMain` and calling the existing C++ `efi_main`.
- Added Windows EDK2 build script.
- Added Linux EDK2 build script.
- Added full EDK2 build documentation.

## Verified

- Host CMake compile-check passes for the C++ layer.

## Not verified in this environment

- Actual EDK2 firmware build, because EDK2 is not installed in the execution container.
- QEMU/OVMF boot, because the EFI binary has to be produced by EDK2 first.
