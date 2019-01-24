#include "streetpass/Message.hpp"

namespace Streetpass {

Message::Message(const std::vector<u8>& buffer) : info(buffer), messageData(buffer) {
}

std::vector<u8> Message::data() const {
    return messageData;
}

MessageInfo& Message::getInfo() {
    return info;
}

const MessageInfo& Message::getInfo() const {
    return info;
}

} // namespace Streetpass
