#include "kernel.hpp"
#include "platform/framebuffer/framebuffer.hpp"
#include "platform/console/text_console.hpp"

extern "C" void KernelMain(const microdos::boot::BootInfo* boot_info) {
    using namespace microdos;

    static platform::Framebuffer framebuffer;
    static platform::TextConsole console;

    if (boot_info != nullptr) {
        framebuffer.initialize(boot_info->framebuffer);
    }

    console.initialize(&framebuffer);
    console.writeLine("MicroDOS Boot");
    console.writeLine("[ OK ] BootInfo received");
    console.writeLine("[ OK ] Framebuffer console initialized");
    console.writeLine("[ OK ] HAL boundary initialized");
    console.writeLine("[ OK ] Shell bootstrap ready");
    console.writeLine("");
    console.writeLine("MicroDOS ready.");
    console.write("A:\\>");

#if defined(MICRODOS_FIRMWARE_BUILD)
    for (;;) {
    #if defined(__clang__) || defined(__GNUC__)
        __asm__ __volatile__("hlt");
    #else
        // Firmware builds are expected to use Clang/GCC-style inline assembly.
    #endif
    }
#else
    // Host compile-check path must return so desktop builds can link and run.
    return;
#endif
}
