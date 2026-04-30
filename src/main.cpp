#include "uefi/uefi.hpp"
#include "uefi/console.hpp"
#include "dos/dos_services.hpp"
#include "shell/shell.hpp"

extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    (void)imageHandle;

    if (!systemTable || !systemTable->ConOut || !systemTable->ConIn) {
        return EFI_INVALID_PARAMETER;
    }

    dosuefi::Console console(*systemTable);
    dosuefi::DosServices dos;
    dosuefi::Shell shell(console, dos);
    shell.run();

    return EFI_SUCCESS;
}
