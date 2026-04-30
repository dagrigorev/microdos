cmake_minimum_required(VERSION 3.25)
project(MicroDOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(MicroDOS_core STATIC
    src/uefi/console.cpp
    src/uefi/file_system.cpp
    src/shell/shell.cpp
    src/fs/fat.cpp
)

target_include_directories(MicroDOS_core PUBLIC src)

if(MSVC)
    target_compile_options(MicroDOS_core PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
    )
else()
    target_compile_options(MicroDOS_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

add_library(microdos_app OBJECT src/main.cpp)
target_link_libraries(microdos_app PRIVATE MicroDOS_core)

# Host compile-check only. Real firmware image is built through EDK2.
