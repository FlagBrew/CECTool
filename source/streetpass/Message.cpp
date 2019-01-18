#include "streetpass/Message.hpp"

namespace Streetpass {

Message::Message(u8* data) : info(data) {
    for (u32 i = 0; i < info.messageSize(); i++)
        {
            messageData.push_back(data[i]);
        }
}

const MessageInfo& Message::getInfo() const {
    return info;
}

MessageInfo& Message::getInfo() {
    return info;
}

std::vector<u8> Message::data() const {
    return messageData;
}

} // namespace Streetpass
