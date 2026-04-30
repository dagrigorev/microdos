#include "fat.hpp"

namespace dosuefi {

static std::uint16_t read16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0] | (p[1] << 8));
}

static std::uint32_t read32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

bool FatVolume::mount(const std::uint8_t* bootSector, std::size_t size) {
    if (!bootSector || size < 36) return false;

    bpb_.bytesPerSector = read16(bootSector + 11);
    bpb_.sectorsPerCluster = bootSector[13];
    bpb_.reservedSectors = read16(bootSector + 14);
    bpb_.fatCount = bootSector[16];
    bpb_.rootEntryCount = read16(bootSector + 17);
    bpb_.totalSectors16 = read16(bootSector + 19);
    bpb_.mediaDescriptor = bootSector[21];
    bpb_.sectorsPerFat = read16(bootSector + 22);
    bpb_.sectorsPerTrack = read16(bootSector + 24);
    bpb_.headCount = read16(bootSector + 26);
    bpb_.hiddenSectors = read32(bootSector + 28);
    bpb_.totalSectors32 = read32(bootSector + 32);

    return bpb_.bytesPerSector != 0 && bpb_.sectorsPerCluster != 0 && bpb_.fatCount != 0;
}

} // namespace dosuefi
