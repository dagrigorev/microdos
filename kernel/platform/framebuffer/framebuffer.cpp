#include "framebuffer.hpp"

namespace microdos::platform {

void Framebuffer::initialize(const boot::FramebufferInfo& info) {
    info_ = info;
}

bool Framebuffer::isReady() const {
    return info_.base != 0 && info_.width != 0 && info_.height != 0 && info_.pixels_per_scanline != 0;
}

void Framebuffer::clear(uint32_t color) {
    if (!isReady()) return;
    auto* pixels = reinterpret_cast<volatile uint32_t*>(info_.base);
    for (uint32_t y = 0; y < info_.height; ++y) {
        for (uint32_t x = 0; x < info_.width; ++x) {
            pixels[y * info_.pixels_per_scanline + x] = color;
        }
    }
}

void Framebuffer::putPixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!isReady() || x >= info_.width || y >= info_.height) return;
    auto* pixels = reinterpret_cast<volatile uint32_t*>(info_.base);
    pixels[y * info_.pixels_per_scanline + x] = color;
}

} // namespace microdos::platform
