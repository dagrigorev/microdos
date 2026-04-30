#pragma once

#include "../uefi/uefi.hpp"

namespace microdos::fs
{
    struct FatBpb
    {
        UINT16 BytesPerSector;
        UINT8  SectorsPerCluster;
        UINT16 ReservedSectors;
        UINT8  FatCount;
    };
}