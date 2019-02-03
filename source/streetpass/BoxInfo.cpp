#include <cstdio>
#include <cstring>

#include "streetpass/BoxInfo.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

BoxInfo::BoxInfo() : boxInfoHeader(), messageHeaders() {
    boxInfoHeader.magic = 0x6262; // 'bb'
}

BoxInfo::BoxInfo(const std::vector<u8>& buffer) : boxInfoHeader(), messageHeaders() {
    std::memcpy(&boxInfoHeader, buffer.data(), sizeof(CecBoxInfoHeader));
    const u32 numMessages = boxInfoHeader.numMessages;
    if (numMessages > 0) {
        messageHeaders.resize(numMessages);
        std::memcpy(messageHeaders.data(), buffer.data() + sizeof(CecBoxInfoHeader), numMessages *
                    sizeof(CecMessageHeader));
    }
    printf("boxInfo constructor\n");
}

BoxInfo::~BoxInfo() {
    printf("boxInfo destructor\n");
}

bool BoxInfo::AddMessageHeader(const CecMessageHeader& messageHeader) {
    if(messageHeader.messageSize + boxInfoHeader.boxSize + sizeof(CecMessageHeader) > boxInfoHeader.maxBoxSize) {
        printf("AddMessageHeader failed: overflow boxsize\n");
    }

    if (boxInfoHeader.numMessages + 1 > boxInfoHeader.maxNumMessages) {
        printf("AddMessageHeader failed: too many messages\n");
        return false;
    }

    for (auto it = messageHeaders.cbegin(); it != messageHeaders.cend(); ++it) {
        if (memcmp(it->messageId, messageHeader.messageId, sizeof(CecMessageId)) == 0) {
            printf("AddMessageHeader failed: message already exists\n");
            return false; // Message found; Adding failed.
        }
    }

    messageHeaders.emplace_back(messageHeader);
    boxInfoHeader.boxSize += messageHeader.messageSize + sizeof(CecMessageHeader);
    boxInfoHeader.fileSize += sizeof(CecMessageHeader);
    boxInfoHeader.numMessages += 1;
    return true;
}

bool BoxInfo::DeleteMessageHeader(const CecMessageId& messageId) {
    for (auto it = messageHeaders.begin(); it != messageHeaders.end(); it++) {
        if(memcmp(it->messageId, messageId.data, sizeof(CecMessageId))) {
            boxInfoHeader.boxSize -= it->messageSize + sizeof(CecMessageHeader);
            boxInfoHeader.fileSize -= sizeof(CecMessageHeader);
            boxInfoHeader.numMessages -= 1;
            messageHeaders.erase(it);
            return true;
        }
    }
    return false;
}

bool BoxInfo::DeleteAllMessageHeaders() {
    for (auto messageHeader : messageHeaders) {
        boxInfoHeader.boxSize -= messageHeader.messageSize + sizeof(CecMessageHeader);
        boxInfoHeader.fileSize -= sizeof(CecMessageHeader);
        boxInfoHeader.numMessages -= 1;
    }
    messageHeaders.clear();
    return true;
}

u32 BoxInfo::BoxSize() const {
    return boxInfoHeader.boxSize;
}

u32 BoxInfo::FileSize() const {
    return boxInfoHeader.fileSize;
}

u32 BoxInfo::MaxBatchSize() const {
    return boxInfoHeader.maxBatchSize;
}

u32 BoxInfo::MaxBoxSize() const {
    return boxInfoHeader.maxBoxSize;
}

u32 BoxInfo::MaxMessages() const {
    return boxInfoHeader.maxNumMessages;
}

u32 BoxInfo::MaxMessageSize() const {
    return boxInfoHeader.maxMessageSize;
}

u32 BoxInfo::NumberOfMessages() const {
    return boxInfoHeader.numMessages;
}

std::vector<u8> BoxInfo::data() const
{
    std::vector<u8> ret(boxInfoHeader.fileSize);
    std::memcpy(ret.data(), &boxInfoHeader, sizeof(CecBoxInfoHeader));
    std::memcpy(ret.data() + sizeof(CecBoxInfoHeader), messageHeaders.data(),
                boxInfoHeader.numMessages * sizeof(CecMessageHeader));
    return ret;
}

CecBoxInfoHeader& BoxInfo::Header() {
    return boxInfoHeader;
}

const CecBoxInfoHeader& BoxInfo::Header() const {
    return boxInfoHeader;
}

std::vector<CecMessageHeader> BoxInfo::MessageHeaders() {
    return messageHeaders;
}

const std::vector<CecMessageHeader> BoxInfo::MessageHeaders() const {
    return messageHeaders;
}

} // namespace Streetpass
