#include <cstring>

#include "streetpass/Message.hpp"

namespace Streetpass {

Message::Message() : messageHeader(), messageData() {

}

Message::Message(const std::vector<u8>& buffer) : messageHeader(), messageData() {
    std::memcpy(&messageHeader, buffer.data(), sizeof(CecMessageHeader));
    const u32 messageDataSize = buffer.size() - sizeof(CecMessageHeader);
    if (messageDataSize > 0) {
        messageData.resize(messageDataSize);
        std::memcpy(messageData.data(), buffer.data() + sizeof(CecMessageHeader), messageDataSize);
    }
}

std::vector<u8> Message::data() const {
    std::vector<u8> ret(sizeof(CecMessageHeader) + messageData.size());
    std::memcpy(ret.data(), &messageHeader, sizeof(CecMessageHeader));
    std::memcpy(ret.data() + sizeof(CecMessageHeader), messageData.data(), messageData.size());
    return ret;
}

u32 Message::MessageSize() const {
    return messageHeader.messageSize;
}

u32 Message::TotalHeaderSize() const {
    return messageHeader.totalHeaderSize;
}

u32 Message::BodySize() const {
    return messageHeader.bodySize;
}

u32 Message::TitleId() const {
    return messageHeader.titleId;
}

u32 Message::TitleId2() const {
    return messageHeader.titleId2;
}

u32 Message::BatchId() const {
    return messageHeader.batchId;
}

u32 Message::Unknown1() const {
    return messageHeader.unknown_1;
}

CecMessageId Message::MessageId() const {
    CecMessageId id;
    std::memcpy(id.data, messageHeader.messageId, sizeof(CecMessageId));
    return id;
}

CecMessageId Message::MessageId2() const {
    CecMessageId id2;
    std::memcpy(id2.data, messageHeader.messageId2, sizeof(CecMessageId));
    return id2;
}

u32 Message::MessageVersion() const {
    return messageHeader.messageVersion;
}

u8 Message::Flags() const {
    return messageHeader.flags;
}

u8 Message::SendMethod() const {
    return messageHeader.sendMethod;
}

u8 Message::Unopened() const {
    return messageHeader.unopened;
}

u8 Message::NewFlag() const {
    return messageHeader.newFlag;
}

u64 Message::SenderId() const {
    return messageHeader.senderId;
}

u64 Message::SenderId2() const {
    return messageHeader.senderId2;
}

CecTimestamp Message::CreatedTime() const {
    return messageHeader.created;
}

CecTimestamp Message::ReceivedTime() const {
    return messageHeader.received;
}

CecTimestamp Message::SentTime() const {
    return messageHeader.sent;
}

u8 Message::SendCount() const {
    return messageHeader.sendCount;
}

u8 Message::ForwardCount() const {
    return messageHeader.forwardCount;
}

u16 Message::UserData() const {
    return messageHeader.userData;
}

const CecMessageHeader& Message::MessageHeader() const {
    return messageHeader;
}

CecMessageHeader& Message::MessageHeader() {
    return messageHeader;
}

} // namespace Streetpass
