#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "streetpass/MessageInfo.hpp"

class Message
{
private:
    MessageInfo info;
    std::vector<u8> messageData;
public:
    Message(u8* data) : info(data)
    {
        for (size_t i = 0; i < info.messageSize(); i++)
        {
            messageData.push_back(data[i]);
        }
    }
    const MessageInfo& getInfo() const { return info; }
    MessageInfo& getInfo() { return info; }
    std::vector<u8> data() const
    {
        return messageData;
    }
};

#endif