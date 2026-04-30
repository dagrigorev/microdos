#pragma once

#include "../uefi/uefi.hpp"

namespace microdos::core
{
    class StringView
    {
    public:
        constexpr StringView()
            : text_(nullptr),
              size_(0)
        {
        }

        constexpr StringView(const CHAR16* text)
            : text_(text),
              size_(LengthOf(text))
        {
        }

        constexpr StringView(const CHAR16* text, UINTN size)
            : text_(text),
              size_(size)
        {
        }

        const CHAR16* Data() const
        {
            return text_;
        }

        UINTN Size() const
        {
            return size_;
        }

        bool Empty() const
        {
            return size_ == 0;
        }

    private:
        static constexpr UINTN LengthOf(const CHAR16* text)
        {
            if (text == nullptr)
            {
                return 0;
            }

            UINTN count = 0;
            while (text[count] != L'\0')
            {
                ++count;
            }

            return count;
        }

        const CHAR16* text_;
        UINTN size_;
    };
}