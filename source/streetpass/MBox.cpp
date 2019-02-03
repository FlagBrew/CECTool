#include <cstring>

#include "streetpass/MBox.hpp"

namespace Streetpass {

MBox::MBox(u32 id) : boxId(id), mboxInfoHeader() {

}

MBox::MBox(const std::string& id) : boxId(std::stoul(id, nullptr, 16)), mboxInfoHeader() {

}

MBox::MBox(const std::vector<u8>& buffer) : boxId(), mboxInfoHeader() {
    std::memcpy(&mboxInfoHeader, buffer.data(), sizeof(CecMBoxInfoHeader));
    boxId = mboxInfoHeader.programId;
}

MBox::MBox(std::unique_ptr<CecInbox> inbox, std::unique_ptr<CecOutbox> outbox,
           std::unique_ptr<MBoxIcon> icon, std::unique_ptr<MBoxProgramId> programId,
           std::unique_ptr<MBoxTitle> title, const std::vector<u8>& infoHeaderBuffer)
           : boxId(), mboxInfoHeader(), inbox(std::move(inbox)), outbox(std::move(outbox)),
           mboxIcon(std::move(icon)), mboxProgramId(std::move(programId)), mboxTitle(std::move(title)) {
    std::memcpy(&mboxInfoHeader, infoHeaderBuffer.data(), sizeof(CecMBoxInfoHeader));
    boxId = mboxInfoHeader.programId;
}

MBox::~MBox() {}

Result MBox::Close() {
    return 0; // Success
}

Result MBox::Open() {
    return 0; // Success
}

Result MBox::Save() {
    return 0; // Success
}

u32 MBox::ProgramId() const {
    return mboxInfoHeader.programId;
}

u32 MBox::PrivateId() const {
    return mboxInfoHeader.privateId;
}

u8 MBox::Flag() const {
    return mboxInfoHeader.flag;
}

u8 MBox::Flag2() const {
    return mboxInfoHeader.flag2;
}

std::array<u8, 32> MBox::HmacKey() const {
    return mboxInfoHeader.hmacKey;
}

CecTimestamp MBox::LastAccessed() const {
    return mboxInfoHeader.lastAccessed;
}

CecTimestamp MBox::LastReceived() const {
    return mboxInfoHeader.lastReceived;
}

CecTimestamp MBox::UnknownTime() const {
    return mboxInfoHeader.unknownTime;
}

std::vector<u8> MBox::data() const {
    std::vector<u8> ret(sizeof(CecMBoxInfoHeader));
    std::memcpy(ret.data(), &mboxInfoHeader, sizeof(CecMBoxInfoHeader));
    return ret;
}

CecMBoxInfoHeader& MBox::Header() {
    return mboxInfoHeader;
}

const CecMBoxInfoHeader& MBox::Header() const {
    return mboxInfoHeader;
}

CecInbox& MBox::Inbox() {
    return *inbox;
}

const CecInbox& MBox::Inbox() const {
    return *inbox;
}

CecOutbox& MBox::Outbox() {
    return *outbox;
}

const CecOutbox& MBox::Outbox() const {
    return *outbox;
}

MBoxIcon& MBox::BoxIcon() {
    return *mboxIcon;
}

const MBoxIcon& MBox::BoxIcon() const {
    return *mboxIcon;
}

MBoxProgramId& MBox::BoxProgramId() {
    return *mboxProgramId;
}

const MBoxProgramId& MBox::BoxProgramId() const {
    return *mboxProgramId;
}

MBoxTitle& MBox::BoxTitle() {
    return *mboxTitle;
}

const MBoxTitle& MBox::BoxTitle() const {
    return *mboxTitle;
}

} // namespace Streetpass
