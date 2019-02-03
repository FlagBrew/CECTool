#pragma once

#include <3ds/types.h>
#include <string>
#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class MBoxTitle {
public:
    explicit MBoxTitle();
    explicit MBoxTitle(const std::vector<u8>& buffer);
    ~MBoxTitle() = default;

    std::vector<u8> data() const;
    u32 size() const;

private:
    const std::string filename = "MBoxData.010";
    std::vector<u8> titleData;
};

} // namespace Streetpass
