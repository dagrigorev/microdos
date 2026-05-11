#include "efi_min.hpp"
#include "../../kernel/boot/boot_info.hpp"
#include "../../kernel/kernel.hpp"

static EFI_SYSTEM_TABLE* gSystemTable = nullptr;

static void fwWrite(const wchar_t* text) {
    if (!gSystemTable || !gSystemTable->ConOut || !text) return;
    gSystemTable->ConOut->OutputString(gSystemTable->ConOut, const_cast<wchar_t*>(text));
}

static microdos::boot::PixelFormat convertPixelFormat(UINT32 uefi_format) {
    // UEFI GOP PixelBlueGreenRedReserved8BitPerColor == 0
    // UEFI GOP PixelRedGreenBlueReserved8BitPerColor == 1
    if (uefi_format == 0) return microdos::boot::PixelFormat::Bgra8;
    if (uefi_format == 1) return microdos::boot::PixelFormat::Rgba8;
    return microdos::boot::PixelFormat::Unknown;
}

static EFI_GRAPHICS_OUTPUT_PROTOCOL* locateGop(EFI_SYSTEM_TABLE* st) {
    void* gop = nullptr;
    EFI_STATUS status = st->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, nullptr, &gop);
    if (EFI_ERROR(status)) return nullptr;
    return reinterpret_cast<EFI_GRAPHICS_OUTPUT_PROTOCOL*>(gop);
}

static EFI_STATUS getMemoryMap(EFI_SYSTEM_TABLE* st, microdos::boot::BootInfo& boot_info, UINTN& map_key) {
    UINTN map_size = 0;
    UINTN descriptor_size = 0;
    UINT32 descriptor_version = 0;
    EFI_STATUS status = st->BootServices->GetMemoryMap(&map_size, nullptr, &map_key, &descriptor_size, &descriptor_version);
    if (status != EFI_BUFFER_TOO_SMALL) return status;

    map_size += descriptor_size * 8;
    void* map = nullptr;
    status = st->BootServices->AllocatePool(EfiLoaderData, map_size, &map);
    if (EFI_ERROR(status)) return status;

    status = st->BootServices->GetMemoryMap(&map_size, reinterpret_cast<EFI_MEMORY_DESCRIPTOR*>(map), &map_key, &descriptor_size, &descriptor_version);
    if (EFI_ERROR(status)) return status;

    boot_info.memory_map.base = reinterpret_cast<uintptr_t>(map);
    boot_info.memory_map.size = map_size;
    boot_info.memory_map.descriptor_size = descriptor_size;
    boot_info.memory_map.descriptor_version = descriptor_version;
    return EFI_SUCCESS;
}

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    (void)image_handle;
    gSystemTable = system_table;

    if (system_table && system_table->ConOut) {
        system_table->ConOut->ClearScreen(system_table->ConOut);
    }

    fwWrite(L"MicroBoot: standalone UEFI loader, no EDK2.\r\n");

    auto* gop = locateGop(system_table);
    if (!gop || !gop->Mode || !gop->Mode->Info) {
        fwWrite(L"MicroBoot: GOP was not found.\r\n");
        return EFI_NOT_FOUND;
    }

    microdos::boot::BootInfo boot_info{};
    boot_info.framebuffer.base = static_cast<uintptr_t>(gop->Mode->FrameBufferBase);
    boot_info.framebuffer.width = gop->Mode->Info->HorizontalResolution;
    boot_info.framebuffer.height = gop->Mode->Info->VerticalResolution;
    boot_info.framebuffer.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    boot_info.framebuffer.bits_per_pixel = 32;
    boot_info.framebuffer.format = convertPixelFormat(gop->Mode->Info->PixelFormat);

    UINTN map_key = 0;
    EFI_STATUS status = getMemoryMap(system_table, boot_info, map_key);
    if (EFI_ERROR(status)) {
        fwWrite(L"MicroBoot: failed to obtain memory map.\r\n");
        return status;
    }

    fwWrite(L"MicroBoot: exiting boot services.\r\n");
    status = system_table->BootServices->ExitBootServices(image_handle, map_key);
    if (EFI_ERROR(status)) {
        // Memory map may have changed once. Retry once.
        status = getMemoryMap(system_table, boot_info, map_key);
        if (!EFI_ERROR(status)) {
            status = system_table->BootServices->ExitBootServices(image_handle, map_key);
        }
    }

    if (EFI_ERROR(status)) {
        fwWrite(L"MicroBoot: ExitBootServices failed.\r\n");
        return status;
    }

    KernelMain(&boot_info);
    return EFI_SUCCESS;
}
