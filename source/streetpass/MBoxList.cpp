#include <cstring>
#include <array>

#include "streetpass/MBoxList.hpp"

namespace Streetpass {

MBoxList::MBoxList() : mboxListHeader() {
    mboxListHeader.magic = 0x6868; // 'hh'
}

MBoxList::MBoxList(const std::vector<u8>& buffer) : mboxListHeader() {
    std::memcpy(&mboxListHeader, buffer.data(), sizeof(CecMBoxListHeader));
}

bool MBoxList::IsSlotUnused(u32 slotNum) const {
    if (slotNum < 0 || slotNum > MaxSlots) {
        return false;
    }
    std::array<u8, 16> emptyName {};
    return memcmp(mboxListHeader.boxNames[slotNum], emptyName.data(), BoxNameSize) == 0;
}

bool MBoxList::IsSlotUsed(u32 slotNum) const {
    if (slotNum < 0 || slotNum > MaxSlots) {
        return false;
    }
    std::array<u8, 16> emptyName {};
    return memcmp(mboxListHeader.boxNames[slotNum], emptyName.data(), BoxNameSize) != 0;
}

u32 MBoxList::NumberOfSlotsUsed() const {
    return mboxListHeader.numBoxes;
}

u32 MBoxList::NumberOfSlotsUnused() const {
    return MaxSlots - mboxListHeader.numBoxes;
}

u32 MBoxList::MaxNumberOfSlots() const {
    return MaxSlots;
}

std::vector<u8> MBoxList::UnusedSlots() const {
    std::vector<u8> unusedSlots;
    for (u32 slotNum = 0; slotNum < MaxSlots; slotNum++) {
        if (IsSlotUnused(slotNum)) {
            unusedSlots.emplace_back(slotNum);
        }
    }
    return unusedSlots;
}

std::vector<u8> MBoxList::UsedSlots() const {
    std::vector<u8> usedSlots;
    for (u32 slotNum = 0; slotNum < MaxSlots; slotNum++) {
        if (IsSlotUsed(slotNum)) {
            usedSlots.emplace_back(slotNum);
        }
    }
    return usedSlots;
}

std::vector<u8> MBoxList::data() const {
    std::vector<u8> ret(sizeof(CecMBoxListHeader));
    std::memcpy(ret.data(), &mboxListHeader, sizeof(CecMBoxListHeader));
    return ret;
}

CecMBoxListHeader& MBoxList::Header() {
    return mboxListHeader;
}

const CecMBoxListHeader& MBoxList::Header() const {
    return mboxListHeader;
}

u32 MBoxList::Version() const {
    return mboxListHeader.version;
}

u32 MBoxList::NumBoxes() const {
    return mboxListHeader.numBoxes;
}

Result MBoxList::AddBox(const std::string& boxId) {
    std::memcpy(mboxListHeader.boxNames[mboxListHeader.numBoxes], boxId.c_str(), boxId.size());
    mboxListHeader.numBoxes += 1;
    return 0;
}

Result MBoxList::DeleteBox(u8 slotNum) {
    std::vector<std::array<u8, 16>> boxList(24);
    std::memcpy(boxList.data(), mboxListHeader.boxNames, sizeof(mboxListHeader.boxNames));
    
    auto it = boxList.begin();
    std::advance(it, slotNum);
    boxList.erase(it);

    std::memcpy(mboxListHeader.boxNames, boxList.data(), sizeof(mboxListHeader.boxNames));
    mboxListHeader.numBoxes -= 1;
    return 0;
}

Result MBoxList::DeleteAllBoxes() {
    CecMBoxListHeader newHeader{};
    newHeader.magic = 0x6868; // 'hh'
    newHeader.version = 0x01;
    
    mboxListHeader = newHeader;

    return 0;
}

std::vector<u32> MBoxList::BoxIds() const {
    std::vector<u32> ret{};
    std::vector<char> boxId(16);
    for (u32 slotNum = 0; slotNum < MaxSlots; slotNum++) {
        std::memcpy(boxId.data(), mboxListHeader.boxNames[slotNum], boxId.size());
        std::string boxName(boxId.data());
        if (boxName.empty()) {
            ret.emplace_back(0);
        } else {
            ret.emplace_back(std::stoul(boxName.c_str(), nullptr, 16));
        }
    }
    return ret;
}

std::vector<std::string> MBoxList::BoxNames() const {
    std::vector<std::string> ret{};
    std::vector<char> boxId(16);
    for (u32 slotNum = 0; slotNum < MaxSlots; slotNum++) {
        std::memcpy(boxId.data(), mboxListHeader.boxNames[slotNum], boxId.size());
        std::string boxName(boxId.data());
        if (boxName.empty()) {
            ret.emplace_back("[Empty] ");
        } else {
            ret.emplace_back(boxName);
        }
    }
    return ret;
}

std::vector<u8> MBoxList::BoxNamesData() const {
    std::vector<u8> ret(sizeof(mboxListHeader.boxNames));
    std::memcpy(ret.data(), &mboxListHeader.boxNames, ret.size());
    return ret;
}

} // namespace Streetpass
