#pragma once

#include "streetpass/BoxInfo.hpp"
#include "streetpass/Message.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class Box {
public:
    virtual ~Box() {};

    virtual Result AddMessage(const Message& message) = 0;
    virtual Result DeleteMessage(const CecMessageId& messageId) = 0;
    virtual Result DeleteAllMessages() = 0;

    virtual BoxInfo& Info() = 0;
    virtual const BoxInfo& Info() const = 0;

    virtual std::vector<Message> Messages() = 0;
    virtual const std::vector<Message> Messages() const = 0;
private:
};

} // namespace Streetpass
