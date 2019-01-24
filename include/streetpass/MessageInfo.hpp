#pragma once

#include <3ds.h>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <array>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

struct Timestamp
{
    u32 year;
    u8 month;
    u8 day;
    u8 weekDay;
    u8 hour;
    u8 minute;
    u8 second;
    u16 millisecond;
};
static_assert(sizeof(Timestamp) == 0x0C, "Timestamp struct as incorrect size.");

struct CecMessageHeader
{
    u16 magic; // 0x6060 ``
    u16 padding;
    u32 messageSize;
    u32 totalHeaderSize;
    u32 bodySize;
    u32 titleID;
    u32 titleID2;
    u32 batchID;
    u32 unknown_1;
    std::array<u8, 8> messageID;
    u32 messageVersion;
    std::array<u8, 8> messageID2;
    u8 flags;
    u8 sendMethod;
    u8 unopened;
    u8 newFlag;
    u64 senderID;
    u64 senderID2;
    Timestamp sent;
    Timestamp received;
    Timestamp created;
    u8 sendCount;
    u8 forwardCount;
    u16 userData;
};
static_assert(sizeof(CecMessageHeader) == 0x70, "CecMessageHeader struct as incorrect size.");

class MessageInfo
{
public:
    explicit MessageInfo(const std::vector<u8>& buffer);
    ~MessageInfo() = default;

    std::vector<u8> data() const;

    cecMessageId messageID() const;
    u32 messageSize() const;

    Timestamp createdTime() const;
    Timestamp receivedTime() const;
    Timestamp sentTime() const;

    void createdTime(const Timestamp& timestamp);
    void receivedTime(const Timestamp& timestamp);
    void sentTime(const Timestamp& timestamp);

    void updateTimes();

private:
    CecMessageHeader messageHeader;
};

} // namespace Streetpass
