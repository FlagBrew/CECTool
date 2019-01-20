#include "streetpass/OBIndex.hpp"

namespace Streetpass {

OBIndex::OBIndex(u8* data, bool cont) {
    std::copy(data, data + 8, (u8*)&obIndex);
        if (cont)
        {
            for (size_t i = 0; i < obIndex.numMessages; i++)
            {
                cecMessageId id;
                std::copy(data + 8 + i * 8, data + 8 + i * 8 + 8, id.data);
                messages.push_back(id);
            }
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
    std::vector<u8> ret;
    u8* info = (u8*)&obIndex;
    for (size_t i = 0; i < sizeof(obIndex); i++)
    {
        ret.push_back(info[i]);
    }
    for (auto id : messages)
    {
        for (size_t i = 0; i < 8; i++)
        {
            ret.push_back(id.data[i]);
        }
    }
    return ret;
}

u32 OBIndex::size() const {
    return obIndex.numMessages;
}

} // namespace Streetpass
