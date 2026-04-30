#pragma once

#include "uefi.hpp"
#include "console.hpp"
#include "../core/string_view.hpp"

namespace microdos {

class UefiFileSystem final {
public:
    UefiFileSystem(EFI_SYSTEM_TABLE& systemTable, EFI_HANDLE imageHandle);

    bool available() const { return root_ != nullptr; }
    EFI_STATUS openRoot();
    EFI_STATUS listDirectory(Console& console, const char16_t* path);
    EFI_STATUS printFile(Console& console, const char16_t* path);

private:
    EFI_STATUS openPath(const char16_t* path, EFI_FILE_PROTOCOL** file);
    static void writeDecimal(Console& console, UINT64 value);
    static bool isSuccess(EFI_STATUS status) { return status == EFI_SUCCESS; }

    EFI_SYSTEM_TABLE& systemTable_;
    EFI_HANDLE imageHandle_ = nullptr;
    EFI_FILE_PROTOCOL* root_ = nullptr;
};

} // namespace microdos
