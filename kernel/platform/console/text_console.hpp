#pragma once

#include "../framebuffer/framebuffer.hpp"

namespace microdos::platform {

class TextConsole {
public:
    void initialize(Framebuffer* framebuffer);
    void clear();
    void write(const char* text);
    void writeLine(const char* text = "");

private:
    void putChar(char c);
    void drawGlyph(char c, uint32_t px, uint32_t py);
    void newline();

    Framebuffer* framebuffer_ = nullptr;
    uint32_t cursor_x_ = 0;
    uint32_t cursor_y_ = 0;
    uint32_t fg_ = 0x00d7e3f4;
    uint32_t bg_ = 0x00000a12;
    static constexpr uint32_t GlyphW = 8;
    static constexpr uint32_t GlyphH = 8;
};

} // namespace microdos::platform
