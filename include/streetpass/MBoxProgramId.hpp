#pragma once

#include <3ds/types.h>
#include <string>
#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class MBoxProgramId {
public:
    explicit MBoxProgramId();
    explicit MBoxProgramId(const std::vector<u8>& buffer);
    ~MBoxProgramId() = default;

    std::vector<u8> data() const;
    u32 size() const;

private:
    const std::string filename = "MBoxData.050";
    std::vector<u8> programIdData;
};

} // namespace Streetpass
