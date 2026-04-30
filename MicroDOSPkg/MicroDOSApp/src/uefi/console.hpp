#pragma once

#include "uefi.hpp"
#include "../core/string_view.hpp"

namespace microdos {

class Console final {
public:
    explicit Console(EFI_SYSTEM_TABLE& systemTable) : systemTable_(systemTable) {}

    void clear();
    void write(const char16_t* text);
    void writeLine(const char16_t* text = u"");
    std::size_t readLine(char16_t* buffer, std::size_t capacity);

private:
    EFI_SYSTEM_TABLE& systemTable_;
};

} // namespace microdos
