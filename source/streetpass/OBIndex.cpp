#include "streetpass/OBIndex.hpp"

namespace Streetpass {

OBIndex::OBIndex(u8* data, bool cont) {
    std::copy(data, data + 8, (u8*)&obIndex);
        if (cont)
        {
            for (size_t i = 0; i < obIndex.messages; i++)
            {
                cecMessageId id;
                std::copy(data + 8 + i * 8, data + 8 + i * 8 + 8, id.data);
                messages.push_back(id);
            }
        }
}

void OBIndex::addMessage(const Message& message) {
    addMessage(message.getInfo());
}

void OBIndex::addMessage(const MessageInfo& message) {
    cecMessageId id = message.messageID();

    if (std::find(messages.cbegin(), messages.cend(), id) == messages.cend())
    {
        messages.push_back(id);
        obIndex.messages++;
    }
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
    return obIndex.messages;
}

} // namespace Streetpass
