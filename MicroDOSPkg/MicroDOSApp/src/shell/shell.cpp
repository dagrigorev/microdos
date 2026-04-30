#include "shell.hpp"
#include "../core/string_view.hpp"

namespace microdos {

void Shell::run() {
    console_.clear();
    console_.writeLine(u"MicroDOS 0.3-edk2 - UEFI-native DOS-compatible shell");
    console_.writeLine(u"MS-DOS 1.25 source-guided compatibility track");
    console_.writeLine(u"Type HELP for commands.");
    console_.writeLine();

    const auto status = fs_.openRoot();
    if (status != EFI_SUCCESS) {
        console_.writeLine(u"Warning: UEFI filesystem is not available yet for this boot context.");
    }

    char16_t line[256]{};
    while (running_) {
        console_.write(u"A> ");
        const auto length = console_.readLine(line, 256);
        execute(U16View(line, length));
    }
}

void Shell::copyArgsToPath(U16View args, char16_t* output, std::size_t capacity) {
    if (!output || capacity == 0) return;
    args = trim(args);
    std::size_t n = args.size();
    if (n + 1 > capacity) n = capacity - 1;
    for (std::size_t i = 0; i < n; ++i) {
        output[i] = args.data()[i] == u'/' ? u'\\' : args.data()[i];
    }
    output[n] = u'\0';
}

void Shell::printStatus(EFI_STATUS status) {
    if (status == EFI_SUCCESS) return;
    switch (status) {
        case EFI_NOT_FOUND: console_.writeLine(u"File not found"); break;
        case EFI_ACCESS_DENIED: console_.writeLine(u"Access denied"); break;
        case EFI_UNSUPPORTED: console_.writeLine(u"Unsupported firmware operation"); break;
        case EFI_NO_MEDIA: console_.writeLine(u"No media"); break;
        default: console_.writeLine(u"Disk or firmware error"); break;
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
    } else if (equalsIgnoreCase(command, U16View(u"REM"))) {
        return;
    } else if (equalsIgnoreCase(command, U16View(u"DIR"))) {
        char16_t path[192]{};
        copyArgsToPath(args.empty() ? U16View(u"\\") : args, path, 192);
        printStatus(fs_.listDirectory(console_, path));
    } else if (equalsIgnoreCase(command, U16View(u"TYPE"))) {
        char16_t path[192]{};
        copyArgsToPath(args, path, 192);
        if (path[0] == u'\0') console_.writeLine(u"Required parameter missing");
        else printStatus(fs_.printFile(console_, path));
    } else if (equalsIgnoreCase(command, U16View(u"DATE"))) {
        console_.writeLine(u"DATE compatibility command reserved; UEFI RuntimeServices time binding is next.");
    } else if (equalsIgnoreCase(command, U16View(u"TIME"))) {
        console_.writeLine(u"TIME compatibility command reserved; UEFI RuntimeServices time binding is next.");
    } else if (equalsIgnoreCase(command, U16View(u"COPY")) || equalsIgnoreCase(command, U16View(u"DEL")) ||
               equalsIgnoreCase(command, U16View(u"ERASE")) || equalsIgnoreCase(command, U16View(u"REN")) ||
               equalsIgnoreCase(command, U16View(u"RENAME")) || equalsIgnoreCase(command, U16View(u"PAUSE"))) {
        console_.writeLine(u"Command recognized from MS-DOS 1.25 COMMAND.ASM, implementation pending.");
    } else {
        console_.writeLine(u"Bad command or file name");
    }
}

void Shell::printHelp() {
    console_.writeLine(u"Available commands:");
    console_.writeLine(u"  DIR       - List files using UEFI SimpleFileSystem");
    console_.writeLine(u"  TYPE      - Print ASCII file contents");
    console_.writeLine(u"  REM       - Comment/no-op, like batch REM");
    console_.writeLine(u"  HELP      - Show this help");
    console_.writeLine(u"  CLS       - Clear screen");
    console_.writeLine(u"  ECHO      - Print text");
    console_.writeLine(u"  VER       - Show compatibility version");
    console_.writeLine(u"  MEM       - Show memory model status");
    console_.writeLine(u"  EXIT      - Return to firmware");
    console_.writeLine();
    console_.writeLine(u"Recognized from COMMAND.ASM, pending: COPY, DEL/ERASE, REN/RENAME, PAUSE, DATE, TIME.");
}

} // namespace microdos
