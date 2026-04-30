# Build Notes

## Recommended path: EDK2

Create an EDK2 application package and include the sources from this repository. The entry point is:

```cpp
extern "C" EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable);
```

## Alternative path: LLVM/LLD

A typical freestanding UEFI build uses PE/COFF output with subsystem EFI application. Exact commands differ by platform and toolchain.

Expected output:

```text
EFI/BOOT/BOOTX64.EFI
```

Place the file on a FAT-formatted EFI System Partition.

## Test with QEMU

```bash
qemu-system-x86_64 \
  -bios OVMF.fd \
  -drive format=raw,file=fat:rw:esp
```

## Test with VirtualBox

Create a VM with EFI enabled and attach a FAT disk image containing:

```text
/EFI/BOOT/BOOTX64.EFI
```
