#include "streetpass/OBIndex.hpp"
#include <cstring>

namespace Streetpass {

OBIndex::OBIndex() : obIndexHeader(), messageIds() {
    obIndexHeader.magic = 0x6767;
}

OBIndex::OBIndex(const std::vector<u8>& buffer) : obIndexHeader(), messageIds() {
    std::memcpy(&obIndexHeader, buffer.data(), sizeof(CecOBIndexHeader));
    const u32 numMessages = obIndexHeader.numMessages;
    if (numMessages > 0) {
        messageIds.resize(numMessages);
        std::memcpy(messageIds.data(), buffer.data() + sizeof(CecOBIndexHeader), numMessages * sizeof(CecMessageId));
    }
    printf("OBIndex Constructor\n");
    printf("numMessages: %lx\n", numMessages);
}

OBIndex::~OBIndex() {
    printf("OBIndex destructor\n");
}

bool OBIndex::AddMessageId(const CecMessageId& messageId) {
    for (auto it = messageIds.cbegin(); it != messageIds.cend(); ++it) {
        if (memcmp(it->data, messageId.data, 8) == 0) {
            return false; // Message id found; Adding failed.
        }
    }
    // Message id not found; Add it to the list
    messageIds.emplace_back(messageId);
    obIndexHeader.numMessages++;
    return true; // Message id added
}

bool OBIndex::DeleteMessageId(const CecMessageId& messageId) {
    for (auto it = messageIds.cbegin(); it != messageIds.cend(); it++) {
        if (memcmp(it->data, messageId.data, 8) == 0) {
            messageIds.erase(it);
            obIndexHeader.numMessages--;
            return true;
        }
    }
    return false;
}

bool OBIndex::DeleteAllMessageIds() {
    obIndexHeader.numMessages = 0;
    messageIds.clear();
    return true;
}

std::vector<u8> OBIndex::data() const {
    std::vector<u8> ret(FileSize());
    std::memcpy(ret.data(), &obIndexHeader, sizeof(CecOBIndexHeader));
    std::memcpy(ret.data() + sizeof(CecOBIndexHeader), messageIds.data(), messageIds.size() * sizeof(CecMessageId));
    return ret;
}

std::vector<CecMessageId> OBIndex::MessageIds() const {
    return messageIds;
}

u32 OBIndex::FileSize() const {
    return sizeof(CecOBIndexHeader) + sizeof(CecMessageId) * obIndexHeader.numMessages;
}

u32 OBIndex::NumberOfMessages() const {
    return obIndexHeader.numMessages;
}

} // namespace Streetpass
