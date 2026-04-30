#pragma once

#include <cstddef>
#include <cstdint>

using UINT8 = std::uint8_t;
using UINT16 = std::uint16_t;
using UINT32 = std::uint32_t;
using UINT64 = std::uint64_t;
using UINTN = std::uintptr_t;
using INT32 = std::int32_t;
using CHAR16 = char16_t;
using EFI_STATUS = UINTN;
using EFI_HANDLE = void*;
using EFI_EVENT = void*;
using EFI_PHYSICAL_ADDRESS = UINT64;
using EFI_VIRTUAL_ADDRESS = UINT64;

constexpr EFI_STATUS EFI_SUCCESS = 0;
constexpr EFI_STATUS EFI_LOAD_ERROR = 1;
constexpr EFI_STATUS EFI_INVALID_PARAMETER = 2;
constexpr EFI_STATUS EFI_UNSUPPORTED = 3;
constexpr EFI_STATUS EFI_NOT_READY = 6;
constexpr EFI_STATUS EFI_DEVICE_ERROR = 7;
constexpr EFI_STATUS EFI_NOT_FOUND = 14;

#define EFIAPI __attribute__((ms_abi))

struct EFI_TABLE_HEADER {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
};

struct SIMPLE_TEXT_OUTPUT_INTERFACE;
struct SIMPLE_INPUT_INTERFACE;
struct EFI_BOOT_SERVICES;
struct EFI_RUNTIME_SERVICES;

using EFI_TEXT_RESET = EFI_STATUS(EFIAPI*)(SIMPLE_TEXT_OUTPUT_INTERFACE*, bool);
using EFI_TEXT_STRING = EFI_STATUS(EFIAPI*)(SIMPLE_TEXT_OUTPUT_INTERFACE*, const CHAR16*);
using EFI_TEXT_CLEAR_SCREEN = EFI_STATUS(EFIAPI*)(SIMPLE_TEXT_OUTPUT_INTERFACE*);

struct SIMPLE_TEXT_OUTPUT_MODE {
    INT32 MaxMode;
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    bool CursorVisible;
};

struct SIMPLE_TEXT_OUTPUT_INTERFACE {
    EFI_TEXT_RESET Reset;
    EFI_TEXT_STRING OutputString;
    void* TestString;
    void* QueryMode;
    void* SetMode;
    void* SetAttribute;
    EFI_TEXT_CLEAR_SCREEN ClearScreen;
    void* SetCursorPosition;
    void* EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE* Mode;
};

struct EFI_INPUT_KEY {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
};

using EFI_INPUT_RESET = EFI_STATUS(EFIAPI*)(SIMPLE_INPUT_INTERFACE*, bool);
using EFI_INPUT_READ_KEY = EFI_STATUS(EFIAPI*)(SIMPLE_INPUT_INTERFACE*, EFI_INPUT_KEY*);

struct SIMPLE_INPUT_INTERFACE {
    EFI_INPUT_RESET Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
};

using EFI_WAIT_FOR_EVENT = EFI_STATUS(EFIAPI*)(UINTN, EFI_EVENT*, UINTN*);

struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void* RaiseTPL;
    void* RestoreTPL;
    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    void* AllocatePool;
    void* FreePool;
    void* CreateEvent;
    void* SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
};

struct EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER Hdr;
    CHAR16* FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    SIMPLE_INPUT_INTERFACE* ConIn;
    EFI_HANDLE ConsoleOutHandle;
    SIMPLE_TEXT_OUTPUT_INTERFACE* ConOut;
    EFI_HANDLE StandardErrorHandle;
    SIMPLE_TEXT_OUTPUT_INTERFACE* StdErr;
    EFI_RUNTIME_SERVICES* RuntimeServices;
    EFI_BOOT_SERVICES* BootServices;
};
