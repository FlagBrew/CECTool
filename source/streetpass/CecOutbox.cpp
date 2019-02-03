#include <cinttypes>

#include "streetpass/CecOutbox.hpp"

namespace Streetpass {

CecOutbox::CecOutbox(u32 id, std::unique_ptr<BoxInfo> outboxInfo, std::unique_ptr<OBIndex> outboxIndex)
    : boxId(id), boxInfo(std::move(outboxInfo)), obIndex(std::move(outboxIndex)), messages() {
    printf("CecOutbox constructor\n");
    if (boxInfo->NumberOfMessages() > 0) {
        for (auto messageHeader : boxInfo->MessageHeaders()) {
            const u32 messageSize = messageHeader.messageSize;
            std::vector<u8> messageBuffer(messageSize);
            Result res = CECDU_ReadMessage(boxId, true, sizeof(CecMessageId), messageSize,
                                           messageHeader.messageId, messageBuffer.data(), nullptr);
            if (R_FAILED(res)) {
                printf("Outbox ReadMessage failed: %" PRIX32 "\n", res);
            } else {
                messages.emplace_back(messageBuffer);
            }
        }
    }
    printf("    boxInfo->NumberOfMessages: %lx\n", boxInfo->NumberOfMessages());
    printf("    obIndex->NumberOfMessages: %lx\n", obIndex->NumberOfMessages());
}

CecOutbox::CecOutbox(u32 id) : boxId(id), messages() {
    u32 outboxInfoSize = 0;
    Result res = CECDU_Open(id, CEC_PATH_OUTBOX_INFO, CEC_READ, &outboxInfoSize);
    if (R_FAILED(res)) {
        printf("Outbox BoxInfo_____ Open failed: %" PRIX32 "\n", res);
        boxInfo = std::make_unique<BoxInfo>();
    } else {
        std::vector<u8> outboxInfoBuffer(outboxInfoSize);
        res = CECDU_OpenAndRead(outboxInfoSize, id, CEC_PATH_OUTBOX_INFO, CEC_READ | CEC_CHECK,
                                outboxInfoBuffer.data(), nullptr);
        if (R_FAILED(res)) {
            printf("Outbox BoxInfo_____ OpenAndRead failed: %" PRIX32 "\n", res);
            boxInfo = std::make_unique<BoxInfo>();
        } else {
            boxInfo = std::make_unique<BoxInfo>(outboxInfoBuffer);
        }
    }

    u32 obIndexSize = 0;
    res = CECDU_Open(id, CEC_PATH_OUTBOX_INDEX, CEC_READ, &obIndexSize);
    if (R_FAILED(res)) {
        printf("OBIndex Open failed: %" PRIX32 "\n", res);
        obIndex = std::make_unique<OBIndex>();
    } else {
        std::vector<u8> obIndexBuffer(obIndexSize);
        res = CECDU_OpenAndRead(obIndexSize, id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK,
                                obIndexBuffer.data(), nullptr);
        if (R_FAILED(res)) {
            printf("OBIndex OpenAndRead failed: %" PRIX32 "\n", res);
            obIndex = std::make_unique<OBIndex>();
        } else {
            obIndex = std::make_unique<OBIndex>(obIndexBuffer);
        }
    }
}

CecOutbox::~CecOutbox() {
    printf("CecOutbox destructor\n");
}

Result CecOutbox::AddMessage(const Message& message) {
    const auto& messageHeader = message.MessageHeader();
    const CecMessageId messageId = message.MessageId();
    const u32 messageSize = message.MessageSize();

    Result res = CECDU_WriteMessage(boxId, true, sizeof(CecMessageId), messageSize,
                                    message.data().data(), messageId.data);
    if (R_FAILED(res)) {
        printf("AddMessage WriteMessage failed: %" PRIX32 "\n", res);
        return res;
    }

    boxInfo->AddMessageHeader(messageHeader);
    obIndex->AddMessageId(messageId);
    return 0;
}

Result CecOutbox::DeleteMessage(const CecMessageId& messageId) {
    Result res = CECDU_Delete(boxId, CEC_PATH_OUTBOX_MSG, true, sizeof(CecMessageId),
                              messageId.data);
    if (R_FAILED(res)) {
        printf("Message Delete failed: %" PRIX32 "\n", res);
        return res;
    }

    boxInfo->DeleteMessageHeader(messageId);
    obIndex->DeleteMessageId(messageId);
    return 0;
}

Result CecOutbox::DeleteAllMessages() {
    
    boxInfo->DeleteAllMessageHeaders();
    obIndex->DeleteAllMessageIds();
    return 0;
}

OBIndex& CecOutbox::Index() {
    return *obIndex;
}

const OBIndex& CecOutbox::Index() const {
    return *obIndex;
}

BoxInfo& CecOutbox::Info() {
    return *boxInfo;
}

const BoxInfo& CecOutbox::Info() const {
    return *boxInfo;
}

std::vector<Message> CecOutbox::Messages() {
    return messages;
}

const std::vector<Message> CecOutbox::Messages() const {
    return messages;
}

} // namespace Streetpass
