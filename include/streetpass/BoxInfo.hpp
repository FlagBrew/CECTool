#ifndef BOXINFO_HPP
#define BOXINFO_HPP

#include <3ds.h>
#include <vector>
#include "streetpass/Message.hpp"

class BoxInfo
{
private:
    u8 info[0x20];
    std::vector<MessageInfo> messages;
public:
    BoxInfo() {}
    BoxInfo(u8* data, bool cont = true)
    {
        std::copy(data, data + 0x20, info);
        if (cont)
        {
            for (size_t i = 0x20; i < fileSize(); i += 0x70)
            {
                messages.push_back({data + i});
            }
        }
    }

    u32 fileSize() const { return *(u32*)(info + 0x4); }
    void fileSize(u32 v) { *(u32*)(info + 0x4) = v; }
    u32 maxBoxSize() const { return *(u32*)(info + 0x8); }
    u32 currentBoxSize() const { return *(u32*)(info + 0xC); }
    void currentBoxSize(u32 v) { *(u32*)(info + 0xC) = v; }
    u32 maxMessages() const { return *(u32*)(info + 0x10); }
    u32 currentMessages() const { return *(u32*)(info + 0x14); }
    void currentMessages(u32 v) { *(u32*)(info + 0x14) = v; }
    u32 maxBatchSize() const { return *(u32*)(info + 0x18); }
    u32 maxMessageSize() const { return *(u32*)(info + 0x1C); }
    bool addMessage(const MessageInfo& message);
    bool addMessage(const Message& message)
    {
        return addMessage(message.getInfo());
    }
    std::vector<u8> data() const;
    const std::vector<MessageInfo>& getMessages() const
    {
        return messages;
    }
    void clearMessages();
    void deleteMessage(cecMessageId id);
};

#endif