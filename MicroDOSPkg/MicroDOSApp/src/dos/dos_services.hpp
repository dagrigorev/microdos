#pragma once

#include <cstddef>
#include <cstdint>

namespace microdos {

struct DosVersion {
    std::uint8_t major = 1;
    std::uint8_t minor = 25;
};

class DosServices final {
public:
    DosVersion version() const { return {}; }
    std::size_t conventionalMemoryKb() const { return 640; }

    // Future INT 21h compatibility surface:
    // - AH=01h keyboard input with echo
    // - AH=02h console output
    // - AH=09h print '$'-terminated string
    // - AH=0Fh open FCB file
    // - AH=10h close FCB file
    // - AH=1Ah set DTA
    // - AH=25h set interrupt vector
    // - AH=26h create PSP
    // - AH=4Ch terminate process
};

} // namespace microdos
