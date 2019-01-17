#pragma once

#include <3ds.h>
#include <vector>
#include "streetpass/Message.hpp"
#include "streetpass/MessageInfo.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

static bool operator == (const cecMessageId& id1, const cecMessageId& id2)
{
    // Isn't this only comparing one byte?
    return !memcmp(&id1, &id2, 1);
}

namespace Streetpass {

class OBIndex
{
private:
    struct
    {
        u8 magic[2];
        u8 padding[2];
        u32 messages;
    } obIndex;
    std::vector<cecMessageId> messages;
public:
    explicit OBIndex(u8* data, bool cont);
    ~OBIndex() = default;

    void addMessage(const Message& message);
    void addMessage(const MessageInfo& message);

    std::vector<u8> data() const;
    u32 size() const;
};

} // namespace Streetpass
