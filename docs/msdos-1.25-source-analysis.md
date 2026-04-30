# MS-DOS 1.25 Source Analysis for UEFI/C++ Port

## Archive contents inspected

The uploaded `MS-DOS 1.25.zip` contains two important folders:

- `v11source` — original assembly sources.
- `v11object` — historical `.COM`, `.EXE`, and BASIC sample binaries.

Source files:

| File | Role in the original system | Porting treatment |
|---|---|---|
| `MSDOS.ASM` | DOS kernel / 86-DOS 1.25 core services | Rewrite as `src/dos` C++ service layer |
| `COMMAND.ASM` | Command interpreter, internal commands, batch logic | Rewrite as `src/shell` |
| `IO.ASM` | Machine/BIOS dependent I/O layer | Replace with `src/uefi` platform layer |
| `TRANS.ASM` | Transient command support | Fold into shell/process layer |
| `ASM.ASM` | Assembler utility | Treat as future `.COM` compatibility test workload |
| `HEX2BIN.ASM` | Utility source | Treat as future `.COM` compatibility test workload |
| `STDDOS.ASM` | Standard DOS glue | Use only as behavioral reference |

## Critical architectural facts from the source

### 1. This cannot be directly recompiled for UEFI

The original code assumes:

- 8086 real mode.
- Segment registers.
- BIOS services.
- Interrupt vector table.
- DOS interrupt calls.
- FCB file API.
- `.COM`/`.EXE` program loading into conventional memory.

UEFI x86_64 does not provide these assumptions. The correct path is a compatibility rewrite.

### 2. COMMAND.ASM internal command table

`COMMAND.ASM` contains the internal command table:

- `DIR`
- `RENAME`
- `REN`
- `ERASE`
- `DEL`
- `TYPE`
- `REM`
- `COPY`
- `PAUSE`
- `DATE`
- `TIME`

This table is now reflected in the C++ shell roadmap. Current implemented/recognized state:

| Command | State |
|---|---|
| `DIR` | Implemented via UEFI SimpleFileSystem |
| `TYPE` | Implemented for ASCII text files via UEFI SimpleFileSystem |
| `REM` | Implemented as no-op |
| `HELP`, `CLS`, `ECHO`, `VER`, `MEM`, `EXIT` | Modern shell helpers |
| `COPY`, `DEL`/`ERASE`, `REN`/`RENAME`, `PAUSE`, `DATE`, `TIME` | Recognized; implementation pending |

### 3. MSDOS.ASM FAT model

`MSDOS.ASM` documents the 32-byte directory entry layout and FAT12 cluster packing. Important compatibility points:

- Directory entry is 32 bytes.
- File name uses 8.3 format.
- Time/date are DOS packed fields.
- First cluster is at offset 26.
- File size is 4 bytes at offset 28.
- FAT entries are 12-bit packed values.
- Cluster numbers start at 2.
- Values greater than `0xFF8` are EOF markers.

The project already has `src/fs/fat.*` for this parser direction.

### 4. Original DOS service boundary

The original system exposes behavior through interrupt/function services. For the C++/UEFI port, those become explicit service methods under `src/dos`.

Priority services:

- Console input/output.
- File open/read/write/close.
- FCB parsing for DOS 1.x compatibility.
- Search first/search next.
- Default drive/current drive.
- Program termination.
- Command tail and PSP creation.

### 5. External program execution

The object folder contains historical binaries such as `COMMAND.COM`, `DEBUG.COM`, `CHKDSK.COM`, `EDLIN.COM`, `FORMAT.COM`, `LINK.EXE`, `SYS.COM`, and BASIC programs.

UEFI cannot run these directly. They become future test inputs for:

- a tiny 8086 interpreter,
- PSP layout emulation,
- selected INT 20h / INT 21h services,
- FCB file I/O compatibility.

## Porting priority after this update

1. Stabilize UEFI filesystem access.
2. Complete DOS-like shell commands from `COMMAND.ASM`.
3. Add FAT12 disk-image reader independent from UEFI SimpleFileSystem.
4. Add FCB parser and file service compatibility.
5. Add `.COM` emulator and use small historical utilities as tests.
