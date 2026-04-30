# Build Notes

## Primary path: EDK2

Use EDK2 to produce the real firmware application:

```powershell
.\scripts\build-edk2-windows.ps1 -Edk2Root C:\src\edk2 -Target DEBUG -ToolChain VS2022
```

Output:

```text
dist/esp/EFI/BOOT/BOOTX64.EFI
```

Full instructions: `docs/build-edk2.md`.

## Host compile-check path

The CMake build is only a syntax/structure check for the portable C++ layer:

```bash
cmake -S . -B build
cmake --build build
```

It does not produce a bootable EFI image.
