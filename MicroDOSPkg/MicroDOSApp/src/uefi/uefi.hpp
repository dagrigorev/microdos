#pragma once

#include <cstddef>
#include <cstdint>

using UINT8 = std::uint8_t;
using UINT16 = std::uint16_t;
using UINT32 = std::uint32_t;
using UINT64 = std::uint64_t;
using UINTN = std::uintptr_t;
using INT16 = std::int16_t;
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
constexpr EFI_STATUS EFI_BAD_BUFFER_SIZE = 4;
constexpr EFI_STATUS EFI_BUFFER_TOO_SMALL = 5;
constexpr EFI_STATUS EFI_NOT_READY = 6;
constexpr EFI_STATUS EFI_DEVICE_ERROR = 7;
constexpr EFI_STATUS EFI_WRITE_PROTECTED = 8;
constexpr EFI_STATUS EFI_OUT_OF_RESOURCES = 9;
constexpr EFI_STATUS EFI_VOLUME_CORRUPTED = 10;
constexpr EFI_STATUS EFI_VOLUME_FULL = 11;
constexpr EFI_STATUS EFI_NO_MEDIA = 12;
constexpr EFI_STATUS EFI_MEDIA_CHANGED = 13;
constexpr EFI_STATUS EFI_NOT_FOUND = 14;
constexpr EFI_STATUS EFI_ACCESS_DENIED = 15;
constexpr EFI_STATUS EFI_END_OF_FILE = 31;

#if defined(_MSC_VER)
#define EFIAPI
#else
#define EFIAPI __attribute__((ms_abi))
#endif

struct EFI_GUID {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
};

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
struct EFI_LOADED_IMAGE_PROTOCOL;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct EFI_FILE_PROTOCOL;

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
using EFI_HANDLE_PROTOCOL = EFI_STATUS(EFIAPI*)(EFI_HANDLE, EFI_GUID*, void**);
using EFI_LOCATE_PROTOCOL = EFI_STATUS(EFIAPI*)(EFI_GUID*, void*, void**);

// Correctly ordered prefix of EFI_BOOT_SERVICES up to LocateProtocol.
// Fields that are not used yet are intentionally opaque to keep this project dependency-light.
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
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;
    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    void* Reserved;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;
    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;
    void* GetNextMonotonicCount;
    void* Stall;
    void* SetWatchdogTimer;
    void* ConnectController;
    void* DisconnectController;
    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;
    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL LocateProtocol;
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

struct EFI_LOADED_IMAGE_PROTOCOL {
    UINT32 Revision;
    EFI_HANDLE ParentHandle;
    EFI_SYSTEM_TABLE* SystemTable;
    EFI_HANDLE DeviceHandle;
    void* FilePath;
    void* Reserved;
    UINT32 LoadOptionsSize;
    void* LoadOptions;
    void* ImageBase;
    UINT64 ImageSize;
    UINT32 ImageCodeType;
    UINT32 ImageDataType;
    void* Unload;
};

constexpr UINT64 EFI_FILE_MODE_READ = 0x0000000000000001ULL;
constexpr UINT64 EFI_FILE_MODE_WRITE = 0x0000000000000002ULL;
constexpr UINT64 EFI_FILE_MODE_CREATE = 0x8000000000000000ULL;

constexpr UINT64 EFI_FILE_READ_ONLY = 0x0000000000000001ULL;
constexpr UINT64 EFI_FILE_HIDDEN = 0x0000000000000002ULL;
constexpr UINT64 EFI_FILE_SYSTEM = 0x0000000000000004ULL;
constexpr UINT64 EFI_FILE_RESERVED = 0x0000000000000008ULL;
constexpr UINT64 EFI_FILE_DIRECTORY = 0x0000000000000010ULL;
constexpr UINT64 EFI_FILE_ARCHIVE = 0x0000000000000020ULL;

struct EFI_TIME {
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight;
    UINT8 Pad2;
};

struct EFI_FILE_INFO {
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64 Attribute;
    CHAR16 FileName[1];
};

using EFI_FILE_OPEN = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, EFI_FILE_PROTOCOL**, const CHAR16*, UINT64, UINT64);
using EFI_FILE_CLOSE = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*);
using EFI_FILE_DELETE = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*);
using EFI_FILE_READ = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, UINTN*, void*);
using EFI_FILE_WRITE = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, UINTN*, void*);
using EFI_FILE_GET_POSITION = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, UINT64*);
using EFI_FILE_SET_POSITION = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, UINT64);
using EFI_FILE_GET_INFO = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, EFI_GUID*, UINTN*, void*);
using EFI_FILE_SET_INFO = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*, EFI_GUID*, UINTN, void*);
using EFI_FILE_FLUSH = EFI_STATUS(EFIAPI*)(EFI_FILE_PROTOCOL*);

struct EFI_FILE_PROTOCOL {
    UINT64 Revision;
    EFI_FILE_OPEN Open;
    EFI_FILE_CLOSE Close;
    EFI_FILE_DELETE Delete;
    EFI_FILE_READ Read;
    EFI_FILE_WRITE Write;
    EFI_FILE_GET_POSITION GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    EFI_FILE_GET_INFO GetInfo;
    EFI_FILE_SET_INFO SetInfo;
    EFI_FILE_FLUSH Flush;
};

using EFI_SIMPLE_FILE_SYSTEM_OPEN_VOLUME = EFI_STATUS(EFIAPI*)(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*, EFI_FILE_PROTOCOL**);

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    UINT64 Revision;
    EFI_SIMPLE_FILE_SYSTEM_OPEN_VOLUME OpenVolume;
};

inline EFI_GUID EfiLoadedImageProtocolGuid() {
    return EFI_GUID{0x5B1B31A1, 0x9562, 0x11d2, {0x8E,0x3F,0x00,0xA0,0xC9,0x69,0x72,0x3B}};
}

inline EFI_GUID EfiSimpleFileSystemProtocolGuid() {
    return EFI_GUID{0x0964E5B22, 0x6459, 0x11D2, {0x8E,0x39,0x00,0xA0,0xC9,0x69,0x72,0x3B}};
}

inline EFI_GUID EfiFileInfoGuid() {
    return EFI_GUID{0x09576E92, 0x6D3F, 0x11D2, {0x8E,0x39,0x00,0xA0,0xC9,0x69,0x72,0x3B}};
}
