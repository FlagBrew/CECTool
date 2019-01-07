#ifndef BOX_HPP
#define BOX_HPP

#include "BoxInfo.hpp"
#include <stdio.h>
#include <string>
extern "C"
{
#include "cecdu.h"
}

class Box
{
private:
    u32 id;
    BoxInfo* info;
    std::vector<cecMessageId> messages;
public:
    Box(u32 id);
    ~Box()
    {
        delete info;
    }
    
    std::vector<std::string> messageNames() const
    {
        std::vector<std::string> ret;
        for (auto message : messages)
        {
            ret.emplace_back((char*)message.data, (char*)message.data + 8);
        }
        return ret;
    }

    std::vector<cecMessageId> getMessages() const { return messages; }
    Result addMessage(Message& message);
    Result clearMessages();
    Result removeMessage(cecMessageId);
};

#endif