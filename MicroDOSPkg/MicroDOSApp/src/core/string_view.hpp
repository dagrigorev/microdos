#pragma once

#include <cstddef>
#include <cstdint>

namespace microdos {

class U16View {
public:
    constexpr U16View() = default;
    constexpr U16View(const char16_t* text) : text_(text), size_(lengthOf(text)) {}
    constexpr U16View(const char16_t* text, std::size_t size) : text_(text), size_(size) {}

    constexpr const char16_t* data() const { return text_; }
    constexpr std::size_t size() const { return size_; }
    constexpr bool empty() const { return size_ == 0; }

private:
    static constexpr std::size_t lengthOf(const char16_t* text) {
        if (!text) return 0;
        std::size_t count = 0;
        while (text[count] != u'\0') ++count;
        return count;
    }

    const char16_t* text_ = u"";
    std::size_t size_ = 0;
};

inline bool equalsIgnoreCase(U16View left, U16View right) {
    if (left.size() != right.size()) return false;
    for (std::size_t i = 0; i < left.size(); ++i) {
        char16_t a = left.data()[i];
        char16_t b = right.data()[i];
        if (a >= u'a' && a <= u'z') a = static_cast<char16_t>(a - 32);
        if (b >= u'a' && b <= u'z') b = static_cast<char16_t>(b - 32);
        if (a != b) return false;
    }
    return true;
}

inline U16View trim(U16View input) {
    std::size_t begin = 0;
    std::size_t end = input.size();
    while (begin < end && (input.data()[begin] == u' ' || input.data()[begin] == u'\t')) ++begin;
    while (end > begin && (input.data()[end - 1] == u' ' || input.data()[end - 1] == u'\t')) --end;
    return U16View(input.data() + begin, end - begin);
}

inline U16View firstToken(U16View input) {
    input = trim(input);
    std::size_t end = 0;
    while (end < input.size() && input.data()[end] != u' ' && input.data()[end] != u'\t') ++end;
    return U16View(input.data(), end);
}

inline U16View afterFirstToken(U16View input) {
    input = trim(input);
    std::size_t pos = 0;
    while (pos < input.size() && input.data()[pos] != u' ' && input.data()[pos] != u'\t') ++pos;
    return trim(U16View(input.data() + pos, input.size() - pos));
}

} // namespace microdos
