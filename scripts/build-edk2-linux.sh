#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: scripts/build-edk2-linux.sh /path/to/edk2 [DEBUG|RELEASE] [GCC5]" >&2
  exit 2
fi

EDK2_ROOT="$(cd "$1" && pwd)"
TARGET="${2:-DEBUG}"
TOOLCHAIN="${3:-GCC5}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

rm -rf "$EDK2_ROOT/MicroDOSPkg"
cp -R "$REPO_ROOT/MicroDOSPkg" "$EDK2_ROOT/MicroDOSPkg"

cd "$EDK2_ROOT"
source edksetup.sh
build -p MicroDOSPkg/MicroDOSPkg.dsc \
      -m MicroDOSPkg/MicroDOSApp/MicroDOSApp.inf \
      -a X64 -t "$TOOLCHAIN" -b "$TARGET"

EFI="$(find "$EDK2_ROOT/Build/MicroDOS" -name MicroDOSApp.efi | head -n 1)"
if [[ -z "$EFI" ]]; then
  echo "MicroDOSApp.efi not found" >&2
  exit 1
fi

mkdir -p "$REPO_ROOT/dist/esp/EFI/BOOT"
cp "$EFI" "$REPO_ROOT/dist/MicroDOSApp.efi"
cp "$EFI" "$REPO_ROOT/dist/esp/EFI/BOOT/BOOTX64.EFI"

echo "EFI app: $EFI"
echo "ESP:     $REPO_ROOT/dist/esp"
