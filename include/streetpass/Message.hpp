#pragma once

#include "streetpass/MessageInfo.hpp"

namespace Streetpass {

class Message
{
private:
    MessageInfo info;
    std::vector<u8> messageData;
public:
    explicit Message(u8* data);
    ~Message() = default;

    const MessageInfo& getInfo() const;
    MessageInfo& getInfo();
    std::vector<u8> data() const;
};

} // namespace Streetpass
