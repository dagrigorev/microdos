#pragma once

#include <stdint.h>
#include <stddef.h>

namespace microdos::boot {

enum class PixelFormat : uint32_t {
    Bgra8 = 0,
    Rgba8 = 1,
    Unknown = 0xffffffffu
};

struct FramebufferInfo {
    uintptr_t base = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t pixels_per_scanline = 0;
    uint32_t bits_per_pixel = 32;
    PixelFormat format = PixelFormat::Unknown;
};

struct MemoryMapInfo {
    uintptr_t base = 0;
    uint64_t size = 0;
    uint64_t descriptor_size = 0;
    uint32_t descriptor_version = 0;
};

struct BootInfo {
    uint32_t magic = 0x4D444F53; // MDOS
    uint32_t version = 1;
    FramebufferInfo framebuffer{};
    MemoryMapInfo memory_map{};
    uintptr_t rsdp = 0;
    uintptr_t initrd_base = 0;
    uint64_t initrd_size = 0;
};

using KernelEntry = void (*)(const BootInfo* boot_info);

} // namespace microdos::boot
