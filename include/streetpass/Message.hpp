#pragma once

#include "streetpass/MessageInfo.hpp"

namespace Streetpass {

class Message
{
public:
    explicit Message(const std::vector<u8>& buffer);
    ~Message() = default;

    std::vector<u8> data() const;

    MessageInfo& getInfo();
    const MessageInfo& getInfo() const;

private:
    MessageInfo info;
    std::vector<u8> messageData;
};

} // namespace Streetpass
