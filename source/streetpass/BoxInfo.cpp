#include "streetpass/BoxInfo.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
#include <cstring>
}

namespace Streetpass {

BoxInfo::BoxInfo() : boxInfoHeader(), messages() {}

BoxInfo::BoxInfo(const std::vector<u8>& buffer, bool cont) : boxInfoHeader(), messages() {
    std::memcpy(&boxInfoHeader, buffer.data(), sizeof(CecBoxInfoHeader));
    if (cont) {
        const u32 numMessages = boxInfoHeader.messageNum;
        for (u32 index = 0; index < numMessages; index++) {
            std::vector<u8> buf(sizeof(CecMessageHeader));
            std::memcpy(buf.data(), buffer.data() + sizeof(CecBoxInfoHeader)
                        + index * sizeof(CecMessageHeader), sizeof(CecMessageHeader));
            messages.emplace_back(buf);
        }
    }
}

std::vector<u8> BoxInfo::data() const
{
    std::vector<u8> ret(fileSize());
    std::memcpy(ret.data(), &boxInfoHeader, sizeof(CecBoxInfoHeader));
    for (u32 index = 0; index < boxInfoHeader.messageNum; index++) {
        std::memcpy(ret.data() + sizeof(CecBoxInfoHeader) + index * sizeof(CecMessageHeader),
                    messages[index].data().data(), sizeof(CecMessageHeader));
    }

    return ret;
}

u32 BoxInfo::currentBoxSize() const {
    return boxInfoHeader.boxSize;
}

u32 BoxInfo::currentMessages() const {
    return boxInfoHeader.messageNum;
}

u32 BoxInfo::fileSize() const {
    return boxInfoHeader.fileSize;
}

u32 BoxInfo::maxBatchSize() const {
    return boxInfoHeader.maxBatchSize;
}

u32 BoxInfo::maxBoxSize() const {
    return boxInfoHeader.maxBoxSize;
}

u32 BoxInfo::maxMessages() const {
    return boxInfoHeader.maxMessageNum;
}

u32 BoxInfo::maxMessageSize() const {
    return boxInfoHeader.maxMessageSize;
}

void BoxInfo::currentBoxSize(u32 size) {
    boxInfoHeader.boxSize = size;
}

void BoxInfo::currentMessages(u32 numMessages) {
    boxInfoHeader.messageNum = numMessages;
}

void BoxInfo::fileSize(u32 size) {
    boxInfoHeader.fileSize = size;
}

bool BoxInfo::addMessage(const Message& message) {
    return addMessage(message.getInfo());
}

bool BoxInfo::addMessage(const MessageInfo& messageInfo)
{
    if(messageInfo.messageSize() + currentBoxSize() + sizeof(CecMessageHeader) > maxBoxSize()) {
        printf("addMessage failed: overflow boxsize\n");
    }

    if (currentMessages() + 1 > maxMessages()) {
        printf("addMessage failed: too many messages\n");
        return false;
    }

    for (auto it = messages.cbegin(); it != messages.cend(); ++it) {
        if (memcmp(it->data().data(), messageInfo.data().data(), sizeof(CecMessageHeader)) == 0) {
            printf("addMessage failed: message already exists\n");
            return false; // Message found; Adding failed.
        }
    }

    currentBoxSize(currentBoxSize() + messageInfo.messageSize() + sizeof(CecMessageHeader));
    messages.push_back(messageInfo);
    fileSize(fileSize() + sizeof(CecMessageHeader));
    currentMessages(currentMessages() + 1);
    return true;
}

void BoxInfo::clearMessages()
{
    messages.clear();
    fileSize(0x20);
    currentBoxSize(0x20);
    currentMessages(0);
}

void BoxInfo::deleteMessage(const CecMessageId& id)
{
    for (auto i = messages.begin(); i != messages.end(); i++)
    {
        if (memcmp(i->messageID().data, id.data, 8) == 0)
        {
            messages.erase(i);
            fileSize(fileSize() - sizeof(CecMessageHeader));
            currentBoxSize(currentBoxSize() - (i->messageSize() + sizeof(CecMessageHeader)));
            currentMessages(currentMessages() - 1);
            return;
        }
    }
}

const std::vector<MessageInfo>& BoxInfo::getMessages() const
{
    return messages;
}

} // namespace Streetpass
