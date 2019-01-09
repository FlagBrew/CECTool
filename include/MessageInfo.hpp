#ifndef MESSAGEINFO_HPP
#define MESSAGEINFO_HPP

#include <3ds.h>
#include <algorithm>
#include <string.h>
extern "C" {
#include "cecdu.h"
}
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
        u32 sentYear;
        u8 sentMonth;
        u8 sentDay;
        u8 sentUnknown[6];
        u32 receivedYear;
        u8 receivedMonth;
        u8 receivedDay;
        u8 receivedUnknown[6];
        u32 createdYear;
        u8 createdMonth;
        u8 createdDay;
        u8 createdUnknown[6];
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
    const u8* data() const { return (u8*)&info; }
    cecMessageId messageID() const;
    // void updateReceivedTime();
    // void updateSentTime();
};

#endif