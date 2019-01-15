#include "streetpass/Box.hpp"
#include <string.h>

Box::Box(u32 id, bool outBox) : id(id), outBox(outBox)
{
    Result res;
    u32 bufferSize = 32;
    u32 readSize = 0;
    u8* in = new u8[bufferSize];
    std::fill_n(in, bufferSize, '\0');

    res = CECDU_OpenAndRead(bufferSize, id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO,
                            CEC_READ | CEC_CHECK, in, &readSize);
    if (R_FAILED(res)) printf("Read info 1\n%X", res);

    info = BoxInfo(in, false);
    if (info.currentMessages() > 0)
    {
        delete[] in;
        bufferSize = info.fileSize();
        in = new u8[bufferSize];

        res = CECDU_OpenAndRead(bufferSize, id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO,
                                CEC_READ | CEC_CHECK, in, &readSize);
        if (R_FAILED(res)) printf("Read info 2\n%X", res);

        info = BoxInfo(in);
    }

    if (outBox)
    {
        delete[] in;
        bufferSize = 8;
        in = new u8[bufferSize];

        res = CECDU_OpenAndRead(bufferSize, id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK, in, &readSize);
        if (R_FAILED(res)) printf("Read OBIndex 1\n%X", res);

        index = new OBIndex(in, false);
        if (index->size() > 0)
        {
            delete[] in;
            bufferSize = 8 + index->size() * 8;
            in = new u8[bufferSize];

            res = CECDU_OpenAndRead(bufferSize, id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK, in, &readSize);
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
    cecMessageId mId = message.getInfo().messageID();
    u32 messageSize = message.getInfo().messageSize();
    return CECDU_WriteMessage(id, outBox, 8, messageSize, message.data().data(), mId.data);
}

Result Box::clearMessages()
{
    Result res;
    for (auto message : info.getMessages())
    {
        cecMessageId mId = message.messageID();
        CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, 8, mId.data);
    }
    info.clearMessages();
    if (R_FAILED(res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, 0, nullptr))) return res;
    auto data = info.data();
    if (R_FAILED(res = CECDU_OpenAndWrite(data.size(), id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_CHECK, data.data()))) return res;
    return 0;
}

Result Box::removeMessage(cecMessageId messageId)
{
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, 8, messageId.data);
    info.deleteMessage(messageId);
    return res;
}

Result Box::saveInfo() const
{
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, 0, nullptr);
    if (R_FAILED(res)) return res;

    auto write = info.data();
    res = CECDU_OpenAndWrite(write.size(), id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_CHECK, write.data());
    if (R_FAILED(res)) return res;

    if (index)
    {
        res = CECDU_Delete(id, CEC_PATH_OUTBOX_INDEX, true, 0, nullptr);
        if (R_FAILED(res)) return res;

        write = index->data();
        res = CECDU_OpenAndWrite(write.size(), id, CEC_PATH_OUTBOX_INDEX, CEC_WRITE | CEC_CHECK, write.data());
    }
    return res;
}