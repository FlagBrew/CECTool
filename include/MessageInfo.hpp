#ifndef MESSAGEINFO_HPP
#define MESSAGEINFO_HPP

#include <3ds.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <ctime>

extern "C" {
#include "3ds/services/cecdu.h"
}

struct Timestamp
{
    u32 year;
    u8 month;
    u8 day;
    u8 weekDay;
    u8 hour;
    u8 minute;
    u8 second;
    u16 millisecond; // Likely, not certain
};

class MessageInfo
{
private:
    struct
    {
        u8 magic[2];
        u8 padding[2];
        u32 messageSize;
        u32 totalHeaderSize;
        u32 bodySize;
        u32 titleID;
        u32 titleID2;
        u32 batchID;
        char unknown_1[4];
        u8 messageID[8];
        u32 messageVersion;
        u8 messageID2[8];
        u8 flags;
        u8 sendMethod;
        u8 unopened;
        u8 newFlag;
        u8 senderID[8];
        u8 senderID2[8];
        Timestamp sent;
        Timestamp received;
        Timestamp created;
        u8 sendCount;
        u8 forwardCount;
        u8 userData[2];
    } info;
public:
    MessageInfo(u8* data)
    {
        std::copy(data, data + 0x70, (u8*)&info);
    }
    bool operator ==(const MessageInfo& message)
    {
        return !(memcmp(&info, &(message.info), sizeof(info)));
    }
    u32 messageSize() const { return info.messageSize; }
    std::vector<u8> data() const
    {
        std::vector<u8> ret;
        u8* infoPtr = (u8*)&info;
        for (size_t i = 0; i < sizeof(info); i++)
        {
            ret.push_back(infoPtr[i]);
        }
        return ret;
    }
    cecMessageId messageID() const;
    Timestamp sentTime() const { return info.sent; }
    Timestamp receivedTime() const { return info.received; }
    Timestamp createdTime() const { return info.created; }
    void sentTime(Timestamp t) { info.sent = t; }
    void receivedTime(Timestamp t) { info.received = t; }
    void updateTimes();
};

#endif