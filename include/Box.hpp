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
public:
    Box(u32 id);
    ~Box()
    {
        delete info;
    }

    std::vector<MessageInfo> getMessages() const { return info->getMessages(); }
    const BoxInfo* getInfo() const { return info; }
    Result addMessage(Message& message);
    Result clearMessages();
    Result removeMessage(cecMessageId);
};

#endif