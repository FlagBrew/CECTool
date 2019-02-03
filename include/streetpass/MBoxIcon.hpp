#pragma once

#include <3ds/types.h>
#include <string>
#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class MBoxIcon {
public:
    explicit MBoxIcon();
    explicit MBoxIcon(const std::vector<u8>& buffer);
    ~MBoxIcon() = default;

    std::vector<u8> data() const;
    u32 size() const;

private:
    const std::string filename = "MBoxData.001";
    std::vector<u8> iconData;
};

} // namespace Streetpass
