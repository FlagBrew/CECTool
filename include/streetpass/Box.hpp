#ifndef BOX_HPP
#define BOX_HPP

#include "streetpass/BoxInfo.hpp"
#include "streetpass/OBIndex.hpp"
#include <stdio.h>
#include <string>

extern "C"
{
#include "3ds/services/cecdu.h"
}

class Box
{
private:
    u32 id;
    BoxInfo info;
    bool outBox;
    OBIndex* index = nullptr;
public:
    Box(u32 id, bool outBox = false);
    ~Box()
    {
        if (index)
        {
            delete index;
        }
    }
    std::vector<MessageInfo> getMessages() const { return info.getMessages(); }
    const BoxInfo& getInfo() const { return info; }
    BoxInfo& getInfo() { return info; }
    Result addMessage(Message& message);
    Result clearMessages();
    Result removeMessage(cecMessageId);
    Result saveInfo() const;
};

#endif