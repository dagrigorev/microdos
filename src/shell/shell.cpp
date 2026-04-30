#include "shell.hpp"
#include "../core/string_view.hpp"

namespace dosuefi {

void Shell::run() {
    console_.clear();
    console_.writeLine(u"DosUefiCpp 0.1 - UEFI-native DOS-compatible shell");
    console_.writeLine(u"Type HELP for commands.");
    console_.writeLine();

    char16_t line[256]{};
    while (running_) {
        console_.write(u"A> ");
        const auto length = console_.readLine(line, 256);
        execute(U16View(line, length));
    }
}

void Shell::execute(U16View line) {
    line = trim(line);
    if (line.empty()) return;

    const auto command = firstToken(line);
    const auto args = afterFirstToken(line);

    if (equalsIgnoreCase(command, U16View(u"HELP"))) {
        printHelp();
    } else if (equalsIgnoreCase(command, U16View(u"CLS"))) {
        console_.clear();
    } else if (equalsIgnoreCase(command, U16View(u"EXIT"))) {
        running_ = false;
    } else if (equalsIgnoreCase(command, U16View(u"VER"))) {
        const auto v = dos_.version();
        (void)v;
        console_.writeLine(u"MS-DOS compatible personality: 1.25");
        console_.writeLine(u"Host: UEFI x86_64 native C++");
    } else if (equalsIgnoreCase(command, U16View(u"MEM"))) {
        (void)dos_.conventionalMemoryKb();
        console_.writeLine(u"Conventional memory model: 640 KB compatibility window planned");
    } else if (equalsIgnoreCase(command, U16View(u"ECHO"))) {
        for (std::size_t i = 0; i < args.size(); ++i) {
            char16_t ch[2] = { args.data()[i], u'\0' };
            console_.write(ch);
        }
        console_.writeLine();
    } else {
        console_.writeLine(u"Bad command or file name");
    }
}

void Shell::printHelp() {
    console_.writeLine(u"Available commands:");
    console_.writeLine(u"  HELP  - Show this help");
    console_.writeLine(u"  CLS   - Clear screen");
    console_.writeLine(u"  ECHO  - Print text");
    console_.writeLine(u"  VER   - Show compatibility version");
    console_.writeLine(u"  MEM   - Show memory model status");
    console_.writeLine(u"  EXIT  - Return to firmware");
    console_.writeLine();
    console_.writeLine(u"Next commands to implement: DIR, TYPE, CD, COPY, DEL, REN.");
}

} // namespace dosuefi
