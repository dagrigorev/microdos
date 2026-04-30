#pragma once

#include <cstdint>
#include <cstddef>

namespace dosuefi {

struct FatBiosParameterBlock {
    std::uint16_t bytesPerSector = 512;
    std::uint8_t sectorsPerCluster = 1;
    std::uint16_t reservedSectors = 1;
    std::uint8_t fatCount = 2;
    std::uint16_t rootEntryCount = 224;
    std::uint16_t totalSectors16 = 0;
    std::uint8_t mediaDescriptor = 0xF0;
    std::uint16_t sectorsPerFat = 0;
    std::uint16_t sectorsPerTrack = 0;
    std::uint16_t headCount = 0;
    std::uint32_t hiddenSectors = 0;
    std::uint32_t totalSectors32 = 0;
};

struct FatDirectoryEntry {
    char name[8]{};
    char extension[3]{};
    std::uint8_t attributes = 0;
    std::uint16_t time = 0;
    std::uint16_t date = 0;
    std::uint16_t firstCluster = 0;
    std::uint32_t fileSize = 0;
};

class FatVolume final {
public:
    bool mount(const std::uint8_t* bootSector, std::size_t size);
    const FatBiosParameterBlock& bpb() const { return bpb_; }

private:
    FatBiosParameterBlock bpb_{};
};

} // namespace dosuefi
