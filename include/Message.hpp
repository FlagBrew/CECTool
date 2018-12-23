#include "MessageInfo.hpp"
class Message
{
private:
    MessageInfo info;
    u8* messageData;
public:
    Message(u8* data) : info(data)
    {
        messageData = new u8[info.messageSize()];
        std::copy(data, data + info.messageSize(), messageData);
    }
    ~Message()
    {
        delete[] messageData;
    }
    const MessageInfo& getInfo() const { return info; }
    u8* data()
    {
        return messageData;
    }
};