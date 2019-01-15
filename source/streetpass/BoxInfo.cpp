#include "streetpass/BoxInfo.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
#include <string.h>
}

bool BoxInfo::addMessage(const MessageInfo& message)
{
    if (message.messageSize() + currentBoxSize() + 0x70 > maxBoxSize() || currentMessages() + 1 > maxMessages() || std::find(messages.begin(), messages.end(), message) != messages.end())
    {
        return false;
    }
    currentBoxSize(currentBoxSize() + message.messageSize() + 0x70);
    messages.push_back(message);
    fileSize(fileSize() + 0x70);
    currentMessages(currentMessages() + 1);
    return true;
}

std::vector<u8> BoxInfo::data() const
{
    std::vector<u8> ret;
    ret.insert(ret.end(), info, info + 0x20);
    for (auto message : messages)
    {
        ret.insert(ret.end(), message.data().begin(), message.data().begin() + 0x70);
    }
    ret.shrink_to_fit();

    return ret;
}

void BoxInfo::clearMessages()
{
    messages.clear();
    fileSize(0x20);
    currentBoxSize(0x20);
    currentMessages(0);
}

void BoxInfo::deleteMessage(cecMessageId id)
{
    for (auto i = messages.begin(); i != messages.end(); i++)
    {
        if (!memcmp(i->messageID().data, id.data, 8))
        {
            messages.erase(i);
            fileSize(fileSize() - 0x70);
            currentBoxSize(currentBoxSize() - (i->messageSize() + 0x70));
            currentMessages(currentMessages() - 1);
            return;
        }
    }
}