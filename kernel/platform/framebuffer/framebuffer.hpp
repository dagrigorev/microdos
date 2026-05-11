#pragma once

#include "../../boot/boot_info.hpp"

namespace microdos::platform {

class Framebuffer {
public:
    void initialize(const boot::FramebufferInfo& info);
    bool isReady() const;
    void clear(uint32_t color);
    void putPixel(uint32_t x, uint32_t y, uint32_t color);
    uint32_t width() const { return info_.width; }
    uint32_t height() const { return info_.height; }

private:
    boot::FramebufferInfo info_{};
};

} // namespace microdos::platform
