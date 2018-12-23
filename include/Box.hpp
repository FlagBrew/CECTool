#include "BoxInfo.hpp"
#include <stdio.h>
extern "C"
{
#include "cecdu.h"
}

class Box
{
private:
    BoxInfo* info;
    std::vector<cecMessageId> messages;
public:
    Box(u32 id)
    {
        Result res;
        u8* in = new u8[32];
        std::fill_n(in, 32, '\0');
        size_t readSize = 32;
        res = CECDU_OpenAndRead(id, CEC_PATH_INBOX_INFO, CEC_READ | CEC_WRITE | CEC_SKIP_CHECKS, in, &readSize);
        if (R_FAILED(res)) printf("Read info 1\n%X", res);
        info = new BoxInfo(in, false);
        if (info->currentMessages() > 0)
        {
            delete[] in;
            in = new u8[readSize = info->fileSize()];
            res = CECDU_OpenAndRead(id, CEC_PATH_INBOX_INFO, CEC_READ | CEC_WRITE | CEC_SKIP_CHECKS, in, &readSize);
            if (R_FAILED(res)) printf("Read info 2\n%X", res);
            delete info;
            info = new BoxInfo(in);
        }
        for (auto message : info->getMessages())
        {
            cecMessageId id;
            std::copy(message.messageID(), message.messageID() + 8, (u8*)&id);
            messages.push_back(id);
        }
    }
    
    std::string messageNames() const
    {
        std::string ret;
        for (auto message : messages)
        {
            ret.append(message.data, message.data + 8).append("\n");
        }
        return ret;
    }
};