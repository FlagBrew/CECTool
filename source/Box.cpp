#include "Box.hpp"
#include <string.h>

Box::Box(u32 id, bool outBox) : id(id), outBox(outBox)
{
    Result res;
    u8* in = new u8[32];
    std::fill_n(in, 32, '\0');
    size_t readSize = 32;
    res = CECDU_OpenAndRead(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_READ | CEC_WRITE | CEC_CHECK, in, &readSize);
    if (R_FAILED(res)) printf("Read info 1\n%X", res);
    info = BoxInfo(in, false);
    if (info.currentMessages() > 0)
    {
        delete[] in;
        in = new u8[readSize = info.fileSize()];
        res = CECDU_OpenAndRead(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_READ | CEC_CHECK, in, &readSize);
        if (R_FAILED(res)) printf("Read info 2\n%X", res);
        info = BoxInfo(in);
    }
    if (outBox)
    {
        delete[] in;
        in = new u8[8];
        readSize = 8;
        res = CECDU_OpenAndRead(id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK, in, &readSize);
        if (R_FAILED(res)) printf("Read OBIndex 1\n%X", res);
        index = new OBIndex(in, false);
        if (index->size() > 0)
        {
            delete[] in;
            in = new u8[readSize = 8 + index->size() * 8];
            res = CECDU_OpenAndRead(id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK, in, &readSize);
            if (R_FAILED(res)) printf("Read OBIndex 2\n%X", res);
            delete index;
            index = new OBIndex(in, true);
        }
    }
    delete[] in;
}

Result Box::addMessage(Message& message)
{
    info.addMessage(message);
    cecMessageId messageId;
    size_t messageSize = message.getInfo().messageSize();
    return CECDU_WriteMessage(id, outBox, message.getInfo().messageID(), message.data().data(), messageSize);
}

Result Box::clearMessages()
{
    Result res;
    for (auto message : info.getMessages())
    {
        CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, message.messageID());
    }
    info.clearMessages();
    if (R_FAILED(res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, {}))) return res;
    auto data = info.data();
    if (R_FAILED(res = CECDU_OpenAndWrite(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_CHECK, data.data(), data.size()))) return res;
    return 0;
}

Result Box::removeMessage(cecMessageId messageId)
{
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, messageId);
    info.deleteMessage(messageId);
    return res;
}

Result Box::saveInfo() const
{
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, {});
    if (R_FAILED(res))
    {
        return res;
    }
    auto write = info.data();
    res = CECDU_OpenAndWrite(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_CHECK, write.data(), write.size());
    if (R_FAILED(res))
    {
        return res;
    }
    if (index)
    {
        res = CECDU_Delete(id, CEC_PATH_OUTBOX_INDEX, true, {});
        if (R_FAILED(res))
        {
            return res;
        }
        write = index->data();
        res = CECDU_OpenAndWrite(id, CEC_PATH_OUTBOX_INDEX, CEC_WRITE | CEC_CHECK, write.data(), write.size());
    }
    return res;
}