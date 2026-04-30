#pragma once

#include "uefi.hpp"
#include "../core/string_view.hpp"

namespace microdos {

static CHAR16* ToEfiString(const char16_t* text)
{
    return const_cast<CHAR16*>(
        reinterpret_cast<const CHAR16*>(text)
    );
}

class Console final {
public:
    explicit Console(EFI_SYSTEM_TABLE& systemTable) : systemTable_(systemTable) {}

    void clear();
    void write(CHAR16* text = nullptr);
    void writeLine(CHAR16* text = nullptr);

    void write(const char16_t* text);
    void writeLine(const char16_t* text);

    UINTN readLine(CHAR16* buffer, UINTN capacity);

private:
    EFI_SYSTEM_TABLE& systemTable_;
};

} // namespace microdos
