#include <3ds.h>
#include <algorithm>
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
        u32 timeSent[3];
        u32 timeReceived[3];
        u32 timeCreated[3];
        u8 sendCount;
        u8 forwardCount;
        u8 userData[2];
    } info;
public:
    MessageInfo(u8* data)
    {
        std::copy(data, data + 0x70, (u8*)&info);
    }
    u32 messageSize() const { return info.messageSize; }
    u8* data() const { return (u8*)&info; }
    const u8* messageID() const { return info.messageID; }
};