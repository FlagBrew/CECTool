#pragma once

#include <3ds/types.h>
#include <array>

namespace Streetpass {

struct MBoxListHeader
{
    u16 magic; // 0x6868 'hh'
    u16 padding;
    u32 version;
    u32 numBoxes;
    //std::array<std::array<u8, 16>, 24> boxIds;
    char boxIds[24][16]; // 12 used, but space for 24
};
static_assert(sizeof(MBoxListHeader) == 0x18C, "MBoxListHeader struct has incorrect size.");

} // namespace Streetpass
