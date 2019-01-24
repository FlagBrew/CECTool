#include "streetpass/OBIndex.hpp"
#include <cstring>

namespace Streetpass {

OBIndex::OBIndex() : obIndex(), messages() {
    obIndex.magic = 0x6767;
}

OBIndex::OBIndex(const std::vector<u8>& buffer) : obIndex(), messages() {
    std::memcpy(&obIndex, buffer.data(), sizeof(OBIndexHeader));
    const u32 numMessages = NumMessages();
    if (numMessages > 0) {
        messages.resize(numMessages);
        std::memcpy(messages.data(), buffer.data() + sizeof(OBIndexHeader), numMessages * sizeof(CecMessageId));
    }
}

bool OBIndex::addMessage(const Message& message) {
    return addMessage(message.getInfo());
}

bool OBIndex::addMessage(const MessageInfo& messageInfo) {
    const CecMessageId id = messageInfo.messageID();

    for (auto it = messages.cbegin(); it != messages.cend(); ++it) {
        if (memcmp(it->data, id.data, 8) == 0) {
            return false; // Message id found; Adding failed.
        }
    }
    // Message id not found; Add it to the list
    messages.push_back(id);
    obIndex.numMessages++;
    return true; // Message id added
}

std::vector<u8> OBIndex::data() const {
    std::vector<u8> ret(FileSize());
    std::memcpy(ret.data(), &obIndex, sizeof(OBIndexHeader));
    std::memcpy(ret.data() + sizeof(OBIndexHeader), messages.data(), messages.size() * sizeof(CecMessageId));
    return ret;
}

u32 OBIndex::FileSize() const {
    return sizeof(OBIndexHeader) + sizeof(CecMessageId) * obIndex.numMessages;
}

u32 OBIndex::NumMessages() const {
    return obIndex.numMessages;
}

} // namespace Streetpass
