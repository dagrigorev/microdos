#pragma once

#include "../uefi/console.hpp"
#include "../uefi/file_system.hpp"
#include "../dos/dos_services.hpp"

namespace microdos {

class Shell final {
public:
    Shell(Console& console, DosServices& dos, UefiFileSystem& fs)
        : console_(console), dos_(dos), fs_(fs) {}
    void run();

private:
    void execute(U16View line);
    void printHelp();
    void printStatus(EFI_STATUS status);
    void copyArgsToPath(U16View args, char16_t* output, std::size_t capacity);

    Console& console_;
    DosServices& dos_;
    UefiFileSystem& fs_;
    bool running_ = true;
};

} // namespace microdos
