#pragma once

#include <3ds.h>
#include <vector>
#include "streetpass/Message.hpp"
#include "streetpass/MessageInfo.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class OBIndex
{
private:
    struct
    {
        u8 magic[2];
        u8 padding[2];
        u32 numMessages;
    } obIndex;
    std::vector<cecMessageId> messages;
public:
    explicit OBIndex(u8* data, bool cont);
    ~OBIndex() = default;

    bool addMessage(const Message& message);
    bool addMessage(const MessageInfo& messageInfo);

    std::vector<u8> data() const;
    u32 size() const;
};

} // namespace Streetpass
