#include "file_system.hpp"

namespace microdos {

UefiFileSystem::UefiFileSystem(EFI_SYSTEM_TABLE& systemTable, EFI_HANDLE imageHandle)
    : systemTable_(systemTable), imageHandle_(imageHandle) {}

EFI_STATUS UefiFileSystem::openRoot() {
    if (root_) return EFI_SUCCESS;
    if (!systemTable_.BootServices || !systemTable_.BootServices->HandleProtocol) {
        return EFI_UNSUPPORTED;
    }

    EFI_LOADED_IMAGE_PROTOCOL* loadedImage = nullptr;
    auto loadedGuid = gEfiLoadedImageProtocolGuid;
    EFI_STATUS status = systemTable_.BootServices->HandleProtocol(
        imageHandle_, &loadedGuid, reinterpret_cast<void**>(&loadedImage));
    if (status != EFI_SUCCESS || !loadedImage || !loadedImage->DeviceHandle) {
        return status == EFI_SUCCESS ? EFI_NOT_FOUND : status;
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* simpleFs = nullptr;
    auto fsGuid = gEfiLoadedImageProtocolGuid;
    status = systemTable_.BootServices->HandleProtocol(
        loadedImage->DeviceHandle, &fsGuid, reinterpret_cast<void**>(&simpleFs));
    if (status != EFI_SUCCESS || !simpleFs || !simpleFs->OpenVolume) {
        return status == EFI_SUCCESS ? EFI_NOT_FOUND : status;
    }

    return simpleFs->OpenVolume(simpleFs, &root_);
}

EFI_STATUS UefiFileSystem::openPath(CHAR16* path, EFI_FILE_PROTOCOL** file) {
    if (!file) return EFI_INVALID_PARAMETER;
    *file = nullptr;
    EFI_STATUS status = openRoot();
    if (status != EFI_SUCCESS) return status;

    if (!path || path[0] == u'\0' || (path[0] == u'\\' && path[1] == u'\0')) {
        *file = root_;
        return EFI_SUCCESS;
    }

    return root_->Open(root_, file, path, EFI_FILE_MODE_READ, 0);
}

void UefiFileSystem::writeDecimal(Console& console, UINT64 value) {
    CHAR16 digits[32]{};
    UINTN used = 0;
    if (value == 0) {
        console.write(u"0");
        return;
    }
    while (value > 0 && used < 32) {
        digits[used++] = static_cast<CHAR16>(L'0' + (value % 10));
        value /= 10;
    }
    while (used > 0) {
        CHAR16 ch[2] = { digits[--used], u'\0' };
        console.write(ch);
    }
}

EFI_STATUS UefiFileSystem::listDirectory(Console& console, CHAR16* path) {
    EFI_FILE_PROTOCOL* dir = nullptr;
    EFI_STATUS status = openPath(path, &dir);
    if (status != EFI_SUCCESS || !dir) return status;

    console.writeLine(u"Directory listing:");

    alignas(EFI_FILE_INFO) UINT8 buffer[1024]{};
    while (true) {
        UINTN size = sizeof(buffer);
        status = dir->Read(dir, &size, buffer);
        if (status != EFI_SUCCESS) break;
        if (size == 0) break;

        auto* info = reinterpret_cast<EFI_FILE_INFO*>(buffer);
        if (info->FileName[0] == u'\0') continue;

        if ((info->Attribute & EFI_FILE_DIRECTORY) != 0) {
            console.write(u"<DIR>      ");
        } else {
            writeDecimal(console, info->FileSize);
            console.write(u" bytes  ");
        }
        console.writeLine(info->FileName);
    }

    if (dir != root_ && dir->Close) dir->Close(dir);
    return status == EFI_SUCCESS ? EFI_SUCCESS : status;
}

EFI_STATUS UefiFileSystem::printFile(Console& console, CHAR16* path) {
    EFI_FILE_PROTOCOL* file = nullptr;
    EFI_STATUS status = openPath(path, &file);
    if (status != EFI_SUCCESS || !file) return status;

    UINT8 buffer[256]{};
    while (true) {
        UINTN size = sizeof(buffer) - 1;
        status = file->Read(file, &size, buffer);
        if (status != EFI_SUCCESS) break;
        if (size == 0) break;

        for (UINTN i = 0; i < size; ++i) {
            CHAR16 ch[2] = { static_cast<CHAR16>(buffer[i]), u'\0' };
            if (buffer[i] == '\n') console.write(u"\r\n");
            else if (buffer[i] != '\r') console.write(ch);
        }
    }

    if (file != root_ && file->Close) file->Close(file);
    console.writeLine();
    return status == EFI_SUCCESS ? EFI_SUCCESS : status;
}

} // namespace microdos
