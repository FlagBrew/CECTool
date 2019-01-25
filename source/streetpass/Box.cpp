#include "streetpass/Box.hpp"
#include <string.h>
#include <inttypes.h>

namespace Streetpass {

Box::Box(u32 id, bool outBox) : id(id), outBox(outBox)
{
    Result res;
    u32 bufferSize = 32;
    u32 readSize = 0;
    std::vector<u8> buffer(bufferSize);

    res = CECDU_OpenAndRead(bufferSize, id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO,
                            CEC_READ | CEC_CHECK, buffer.data(), &readSize);
    if (R_FAILED(res)) {
        printf("OpenAndRead info 1 failed: %" PRIX32 "\n", res);
    }

    info = BoxInfo(buffer, false);
    if (info.currentMessages() > 0)
    {
        bufferSize = info.fileSize();
        buffer.resize(bufferSize);

        res = CECDU_OpenAndRead(bufferSize, id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO,
                                CEC_READ | CEC_CHECK, buffer.data(), &readSize);
        if (R_FAILED(res)) {
            printf("OpenAndRead info 2 failed: %" PRIX32 "\n", res);
        }

        info = BoxInfo(buffer);
    }

    if (outBox)
    {
        u32 obIndexSize = 0;
        res = CECDU_Open(id, CEC_PATH_OUTBOX_INDEX, CEC_READ, &obIndexSize);
        if (R_FAILED(res)) {
            printf("OBIndex Open failed: %" PRIX32 "\n", res);
            obIndex = std::make_unique<OBIndex>();
        } else {
            std::vector<u8> obIndexBuffer(obIndexSize);
            res = CECDU_OpenAndRead(obIndexSize, id, CEC_PATH_OUTBOX_INDEX, CEC_READ, obIndexBuffer.data(), &readSize);
            if (R_FAILED(res)) {
                printf("OBIndex OpenAndRead failed: %" PRIX32 "\n", res);
            } else {
                obIndex = std::make_unique<OBIndex>(obIndexBuffer);
            }
        }
    }
}

Result Box::addMessage(const Message& message)
{
    // Should use SetData option 2 to update OBIndex after each message, with messageid in the buffer.
    cecMessageId mId = message.getInfo().messageID();
    u32 messageSize = message.getInfo().messageSize();

    info.addMessage(message);
    obIndex->addMessage(message);

    Result res = CECDU_WriteMessage(id, outBox, 8, messageSize, message.data().data(), mId.data);
    if (R_FAILED(res)) {
        printf("Message WriteMessage failed: %" PRIX32 "\n", res);
        return res;
    }
    return 0;
}

Result Box::clearMessages()
{
    Result res;
    for (auto message : info.getMessages())
    {
        cecMessageId mId = message.messageID();
        res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, 8, mId.data);
        if (R_FAILED(res)) {
            printf("Message Delete failed: %" PRIX32 "\n", res);
            return res;
        }
    }
    info.clearMessages();

    res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, 0, nullptr);
    if (R_FAILED(res)) {
        printf("BoxInfo Delete failed: %" PRIX32 "\n", res);
        return res;
    }

    auto data = info.data();
    res = CECDU_OpenAndWrite(data.size(), id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_CHECK, data.data());
    if (R_FAILED(res)) {
        printf("BoxInfo OpenAndWrite failed: %" PRIX32 "\n", res);
        return res;
    }
    return 0;
}

Result Box::removeMessage(cecMessageId messageId)
{
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_MSG : CEC_PATH_INBOX_MSG, outBox, 8, messageId.data);
    if (R_FAILED(res)) {
        printf("Message Delete failed: %" PRIX32 "\n", res);
        return res;
    }
    //if (outBox) box->removeMessage(messageId);
    info.deleteMessage(messageId);
    return 0;
}

Result Box::saveInfo() const
{
    // Since we are going through CECD, we don't have to delete the files everytime.
    Result res = CECDU_Delete(id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, outBox, 0, nullptr);
    if (R_FAILED(res)) {
        printf("BoxInfo Delete failed: %" PRIX32 "\n", res);
        return res;
    }

    auto write = info.data();
    res = CECDU_OpenAndWrite(write.size(), id, outBox ? CEC_PATH_OUTBOX_INFO : CEC_PATH_INBOX_INFO, CEC_CREATE | CEC_WRITE | CEC_CHECK, write.data());
    if (R_FAILED(res)) {
        printf("BoxInfo OpenAndWrite failed: %" PRIX32 "\n", res);
        return res;
    }

    if (obIndex && outBox)
    {
        res = CECDU_Delete(id, CEC_PATH_OUTBOX_INDEX, true, 0, nullptr);
        if (R_FAILED(res)) {
            printf("OBIndex Delete failed: %" PRIX32 "\n", res);
            return res;
        }

        std::vector<u8> obIndexBuffer = obIndex->data();
        res = CECDU_OpenAndWrite(obIndexBuffer.size(), id, CEC_PATH_OUTBOX_INDEX,
                                 CEC_WRITE | CEC_CHECK | CEC_CREATE, obIndexBuffer.data());
        if (R_FAILED(res)) {
            printf("OBIndex OpenAndWrite failed: %" PRIX32 "\n", res);
            return res;
        }
    }
    return 0;
}

OBIndex& Box::Index() {
    return *obIndex;
}

const OBIndex& Box::Index() const {
    return *obIndex;
}

} // namespace Streetpass
