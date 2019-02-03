#pragma once

#include <memory>

#include <3ds/types.h>

#include "streetpass/MBox.hpp"
#include "streetpass/MBoxList.hpp"

namespace Streetpass {

class StreetpassManager {
public:
    explicit StreetpassManager();
    ~StreetpassManager();

    Result HexDump(const std::vector<u8>& buffer) const;
    Result ListBoxes() const;

    std::unique_ptr<MBox> OpenBox(u8 slotNum) const;

    MBoxList& BoxList();
    const MBoxList& BoxList() const;

private:
    std::unique_ptr<MBoxList> mboxList;
};

} // namespace Streetpass
