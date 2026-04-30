# Next Implementation Backlog

## Commit 1 — UEFI file commands

Status: started in this archive.

- Add UEFI LoadedImageProtocol lookup.
- Add SimpleFileSystemProtocol lookup from the loaded image device.
- Add root volume opening.
- Add `DIR`.
- Add `TYPE`.

## Commit 2 — COMMAND.ASM parity pass

- Implement `PAUSE`.
- Implement `COPY` for firmware filesystem files.
- Implement `DEL` / `ERASE`.
- Implement `REN` / `RENAME`.
- Add strict DOS-style error messages.

## Commit 3 — date/time

- Bind UEFI RuntimeServices `GetTime`.
- Implement `DATE` display.
- Implement `TIME` display.
- Do not modify firmware clock by default.

## Commit 4 — FAT12 image reader

- Load a disk image file from the EFI partition.
- Parse BPB.
- Parse root directory.
- Read cluster chains.
- Support 8.3 lookup.

## Commit 5 — DOS kernel service surface

- Replace ad-hoc shell filesystem calls with `DosServices` file APIs.
- Add DOS error code enum.
- Add FCB parser.
- Add DTA/search result structure.

## Commit 6 — `.COM` emulator phase 1

- 1 MB address space.
- 8086 registers and flags.
- Load `.COM` at offset `0x100`.
- Implement INT 20h terminate.
- Implement INT 21h console output functions.

## Commit 7 — `.COM` emulator phase 2

- Implement enough instructions for tiny hand-made `.COM` tests.
- Then attempt selected historical utilities from `v11object`.
