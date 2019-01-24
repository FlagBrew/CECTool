#pragma once

#include <3ds/types.h>
#include <vector>
#include "streetpass/Message.hpp"

namespace Streetpass {

struct CecBoxInfoHeader {
    u16 magic; // 0x6262 'bb'
    u16 padding;
    u32 fileSize;
    u32 maxBoxSize;
    u32 boxSize;
    u32 maxMessageNum;
    u32 messageNum;
    u32 maxBatchSize;
    u32 maxMessageSize;
};
static_assert(sizeof(CecBoxInfoHeader) == 0x20, "CecBoxInfoHeader struct has incorrect size.");

class BoxInfo
{
public:
    explicit BoxInfo();
    explicit BoxInfo(const std::vector<u8>& buffer, bool cont = true);
    ~BoxInfo() = default;

    std::vector<u8> data() const;

    u32 currentBoxSize() const;
    u32 currentMessages() const;
    u32 fileSize() const;
    u32 maxBatchSize() const;
    u32 maxBoxSize() const;
    u32 maxMessages() const;
    u32 maxMessageSize() const;

    void currentBoxSize(u32 size);
    void currentMessages(u32 numMessages);
    void fileSize(u32 size);

    bool addMessage(const Message& message);
    bool addMessage(const MessageInfo& messageInfo);

    void clearMessages();
    void deleteMessage(const CecMessageId& id);

    const std::vector<MessageInfo>& getMessages() const;

private:
    CecBoxInfoHeader boxInfoHeader;
    std::vector<MessageInfo> messages;
};

} // namespace Streetpass
