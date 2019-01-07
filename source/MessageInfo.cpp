#include "MessageInfo.hpp"
extern "C" {
#include "cecdu.h"
}

cecMessageId MessageInfo::messageID() const
{
    cecMessageId id;
    std::copy(info.messageID, info.messageID + 8, id.data);
    return id;
}