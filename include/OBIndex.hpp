#ifndef OBINDEX_HPP
#define OBINDEX_HPP

#include <3ds.h>
#include <vector>
#include "Message.hpp"
#include "MessageInfo.hpp"

extern "C"
{
#include "cecdu.h"
}

static bool operator == (const cecMessageId& id1, const cecMessageId& id2)
{
    return !memcmp(&id1, &id2, 1);
}

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
    OBIndex(u8* data, bool cont)
    {
        std::copy(data, data + 8, (u8*)&obIndex);
        if (cont)
        {
            for (size_t i = 0; i < obIndex.messages; i++)
            {
                cecMessageId id;
                std::copy(data + 8 + i * 8, data + 8 + i * 8 + 8, id.data);
                messages.push_back(id);
            }
        }
    }

    std::vector<u8> data() const
    {
        std::vector<u8> ret;
        for (size_t i = 0; i < sizeof(obIndex); i++)
        {
            ret.push_back(*(((u8*)&obIndex) + i));
        }
        for (auto id : messages)
        {
            for (size_t i = 0; i < 8; i++)
            {
                ret.push_back(id.data[i]);
            }
        }
        return ret;
    }
    void addMessage(const MessageInfo& message)
    {
        cecMessageId id = message.messageID();
        if (std::find(messages.cbegin(), messages.cend(), id) == messages.end())
        {
            messages.push_back(id);
            obIndex.messages++;
        }
    }
    void addMessage(const Message& message)
    {
        addMessage(message.getInfo());
    }
    u32 size() const { return obIndex.messages; }
};

#endif