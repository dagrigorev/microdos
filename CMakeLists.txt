cmake_minimum_required(VERSION 3.25)
project(MicroDOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dosuefi_core STATIC
    src/uefi/console.cpp
    src/shell/shell.cpp
    src/fs/fat.cpp
)

target_include_directories(dosuefi_core PUBLIC src)

target_compile_options(dosuefi_core PRIVATE
    -ffreestanding
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
    -Wpedantic
)

add_library(dosuefi_app OBJECT src/main.cpp)
target_link_libraries(dosuefi_app PRIVATE dosuefi_core)

# The final PE/COFF EFI link step is toolchain-specific.
# See docs/build.md for EDK2 / LLVM / GNU-EFI integration notes.
