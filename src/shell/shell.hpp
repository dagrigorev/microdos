#pragma once

#include "../uefi/console.hpp"
#include "../dos/dos_services.hpp"

namespace dosuefi {

class Shell final {
public:
    Shell(Console& console, DosServices& dos) : console_(console), dos_(dos) {}
    void run();

private:
    void execute(U16View line);
    void printHelp();

    Console& console_;
    DosServices& dos_;
    bool running_ = true;
};

} // namespace dosuefi
