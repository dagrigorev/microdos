#pragma once

#include "boot/boot_info.hpp"

extern "C" void KernelMain(const microdos::boot::BootInfo* boot_info);
