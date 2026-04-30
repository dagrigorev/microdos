#include "uefi/uefi.hpp"
#include "uefi/console.hpp"
#include "uefi/file_system.hpp"
#include "dos/dos_services.hpp"
#include "shell/shell.hpp"

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    if (!systemTable || !systemTable->ConOut || !systemTable->ConIn) {
        return EFI_INVALID_PARAMETER;
    }

    microdos::Console console(*systemTable);
    microdos::UefiFileSystem fs(*systemTable, imageHandle);
    microdos::DosServices dos;
    microdos::Shell shell(console, dos, fs);
    shell.run();

    return EFI_SUCCESS;
}
