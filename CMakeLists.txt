cmake_minimum_required(VERSION 3.25)
project(MicroDOSStandalone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MICRODOS_BUILD_HOST_CHECK "Build host compile-check executable" ON)

add_library(microdos_kernel STATIC
    kernel/kernel.cpp
    kernel/platform/framebuffer/framebuffer.cpp
    kernel/platform/console/text_console.cpp
)

target_include_directories(microdos_kernel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel
)

if(MSVC)
    target_compile_options(microdos_kernel PRIVATE /W4 /permissive- /Zc:__cplusplus)
else()
    target_compile_options(microdos_kernel PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MICRODOS_BUILD_HOST_CHECK)
    add_executable(microdos_host_check tools/host_check.cpp)
    target_link_libraries(microdos_host_check PRIVATE microdos_kernel)
endif()
