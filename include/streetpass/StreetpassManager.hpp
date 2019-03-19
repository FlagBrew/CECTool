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

    std::shared_ptr<MBox> OpenBox(u8 slotNum) const;

    Result CreateBox(u32 boxId, const std::string& boxName, std::unique_ptr<MBox> mbox);
    Result DeleteBox(u8 slotNum);
    Result DeleteAllBoxes();
    void BackupBox(u8 slotNum);
    Result ImportBox(u32 boxId);

    MBoxList& BoxList();
    const MBoxList& BoxList() const;

    Result ReloadBoxList();

private:
    std::unique_ptr<MBoxList> mboxList;
    mutable std::array<std::shared_ptr<MBox>, 12> boxes;
};

} // namespace Streetpass
