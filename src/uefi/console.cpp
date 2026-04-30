#include "console.hpp"

namespace dosuefi {

void Console::clear() {
    if (systemTable_.ConOut && systemTable_.ConOut->ClearScreen) {
        systemTable_.ConOut->ClearScreen(systemTable_.ConOut);
    }
}

void Console::write(const char16_t* text) {
    if (systemTable_.ConOut && systemTable_.ConOut->OutputString && text) {
        systemTable_.ConOut->OutputString(systemTable_.ConOut, text);
    }
}

void Console::writeLine(const char16_t* text) {
    write(text);
    write(u"\r\n");
}

std::size_t Console::readLine(char16_t* buffer, std::size_t capacity) {
    if (!buffer || capacity == 0) return 0;

    std::size_t length = 0;
    buffer[0] = u'\0';

    while (true) {
        UINTN index = 0;
        EFI_EVENT event = systemTable_.ConIn ? systemTable_.ConIn->WaitForKey : nullptr;
        if (systemTable_.BootServices && systemTable_.BootServices->WaitForEvent && event) {
            systemTable_.BootServices->WaitForEvent(1, &event, &index);
        }

        EFI_INPUT_KEY key{};
        if (!systemTable_.ConIn || systemTable_.ConIn->ReadKeyStroke(systemTable_.ConIn, &key) != EFI_SUCCESS) {
            continue;
        }

        if (key.UnicodeChar == u'\r') {
            write(u"\r\n");
            buffer[length] = u'\0';
            return length;
        }

        if (key.UnicodeChar == u'\b') {
            if (length > 0) {
                --length;
                buffer[length] = u'\0';
                write(u"\b \b");
            }
            continue;
        }

        if (key.UnicodeChar >= 32 && length + 1 < capacity) {
            buffer[length++] = key.UnicodeChar;
            buffer[length] = u'\0';
            char16_t echo[2] = { key.UnicodeChar, u'\0' };
            write(echo);
        }
    }
}

} // namespace dosuefi
