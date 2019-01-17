#pragma once

#include <3ds/types.h>

namespace Streetpass {

struct boxList
{
    u8 magic[2];
    u8 padding[2];
    u32 version;
    u32 numBoxes;
    char boxId[24][16]; // 12 used, but space for 24
};
static_assert(sizeof(boxList) == 0x18C, "boxList struct has incorrect size."); // 396 bytes

} // namespace Streetpass
