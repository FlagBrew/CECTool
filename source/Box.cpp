#include "Box.hpp"
#include <string.h>

Box::Box(u32 id) : id(id)
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
        messages.push_back(message.messageID());
    }
}

Result Box::addMessage(Message& message)
{
    info->addMessage(message);
    cecMessageId messageId;
    size_t messageSize = message.getInfo().messageSize() + 0x70;
    return CECDU_WriteMessage(id, false, message.getInfo().messageID(), const_cast<u8*>(message.data().data()), messageSize);
}

Result Box::clearMessages()
{
    Result res;
    for (auto message : info->getMessages())
    {
        CECDU_Delete(id, CEC_PATH_INBOX_MSG, false, message.messageID());
    }
    info->clearMessages();
    if (R_FAILED(res = CECDU_Delete(id, CEC_PATH_INBOX_INFO, false, {}))) return res;
    auto data = info->data();
    if (R_FAILED(res = CECDU_OpenAndWrite(id, CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_SKIP_CHECKS, data.data(), data.size()))) return res;
    return 0;
}

Result Box::removeMessage(cecMessageId messageId)
{
    Result res = CECDU_Delete(id, CEC_PATH_INBOX_MSG, false, messageId);
    info->deleteMessage(messageId);
    return res;
}