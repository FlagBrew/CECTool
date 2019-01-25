#pragma once

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include "streetpass/BoxInfo.hpp"
#include "streetpass/OBIndex.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class Box
{

public:
    Box(u32 id, bool outBox = false);
    ~Box() = default;
    std::vector<MessageInfo> getMessages() const { return info.getMessages(); }

    const BoxInfo& getInfo() const { return info; }
    BoxInfo& getInfo() { return info; }

    Result addMessage(const Message& message);
    Result clearMessages();
    Result removeMessage(cecMessageId);
    Result saveInfo() const;

    OBIndex& Index();
    const OBIndex& Index() const;

private:
    u32 id;
    bool outBox;
    BoxInfo info;
    //std::unique_ptr<BoxInfo> info;
    std::unique_ptr<OBIndex> obIndex;
};

} // namespace Streetpass
