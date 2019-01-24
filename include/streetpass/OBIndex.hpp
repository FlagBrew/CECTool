#pragma once

#include <3ds/types.h>
#include <vector>
#include "streetpass/Message.hpp"
#include "streetpass/MessageInfo.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

struct OBIndexHeader
{
    u16 magic;
    u16 padding;
    u32 numMessages;
};
static_assert(sizeof(OBIndexHeader) == 0x08, "OBIndexHeader struct as incorrect size.");

class OBIndex
{
public:
    explicit OBIndex();
    explicit OBIndex(const std::vector<u8>& buffer);
    ~OBIndex() = default;

    bool addMessage(const Message& message);
    bool addMessage(const MessageInfo& messageInfo);

    std::vector<u8> data() const;

    u32 FileSize() const;
    u32 NumMessages() const;

private:
    OBIndexHeader obIndex;
    std::vector<cecMessageId> messages;
};

} // namespace Streetpass
