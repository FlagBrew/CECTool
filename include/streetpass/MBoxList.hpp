#pragma once

#include <3ds/types.h>
#include <string>
#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class MBoxList {
public:
    explicit MBoxList();
    explicit MBoxList(const std::vector<u8>& buffer);
    ~MBoxList() = default;

    bool IsSlotUnused(u32 slotNum) const;
    bool IsSlotUsed(u32 slotNum) const;

    u32 NumberOfSlotsUsed() const;
    u32 NumberOfSlotsUnused() const;
    u32 MaxNumberOfSlots() const;

    std::vector<u8> UnusedSlots() const;
    std::vector<u8> UsedSlots() const;

    Result AddBox(const std::string& boxId);

    Result DeleteBox(u8 slotNum);
    Result DeleteAllBoxes();

    std::vector<u32> BoxIds() const;
    std::vector<std::string> BoxNames() const;

    std::vector<u8> BoxNamesData() const;
    u32 NumBoxes() const;
    u32 Version() const;

    std::vector<u8> data() const;

    CecMBoxListHeader& Header();
    const CecMBoxListHeader& Header() const;
private:
    CecMBoxListHeader mboxListHeader;

    const std::string filename = "MBoxList____";

    static constexpr u32 MaxSlots = 12;
    static constexpr u32 BoxNameSize = 16;
};

} // namespace Streetpass
