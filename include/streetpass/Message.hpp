#pragma once

#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class Message {
public:
    explicit Message();
    explicit Message(const std::vector<u8>& buffer);
    ~Message() = default;

    std::vector<u8> data() const;

    u32 MessageSize() const;
    u32 TotalHeaderSize() const;
    u32 BodySize() const;
    u32 TitleId() const;
    u32 TitleId2() const;
    u32 BatchId() const;
    u32 Unknown1() const;
    CecMessageId MessageId() const;
    u32 MessageVersion() const;
    CecMessageId MessageId2() const;
    u8 Flags() const;
    u8 SendMethod() const;
    u8 Unopened() const;
    u8 NewFlag() const;
    u64 SenderId() const;
    u64 SenderId2() const;
    CecTimestamp SentTime() const;
    CecTimestamp ReceivedTime() const;
    CecTimestamp CreatedTime() const;
    u8 SendCount() const;
    u8 ForwardCount() const;
    u16 UserData() const;

    const CecMessageHeader& MessageHeader() const;
    CecMessageHeader& MessageHeader();

private:
    CecMessageHeader messageHeader;
    std::vector<u8> messageData;
};

} // namespace Streetpass
