#include <cinttypes>

#include "streetpass/CecInbox.hpp"

namespace Streetpass {

CecInbox::CecInbox(u32 id, std::unique_ptr<BoxInfo> inboxInfo) : boxId(id),
                   boxInfo(std::move(inboxInfo)), messages() {
    printf("CecInbox constructor\n");
    if (boxInfo->NumberOfMessages() > 0) {
        for (auto messageHeader : boxInfo->MessageHeaders()) {
            const u32 messageSize = messageHeader.messageSize;
            std::vector<u8> messageBuffer(messageSize);
            Result res = CECDU_ReadMessage(boxId, false, sizeof(CecMessageId), messageSize,
                                           messageHeader.messageId, messageBuffer.data(), nullptr);
            if (R_FAILED(res)) {
                printf("Inbox ReadMessage failed: %" PRIX32 "\n", res);
            } else {
                messages.emplace_back(messageBuffer);
            }
        }
    }
    printf("    boxInfo->NumberOfMessages: %lx\n", boxInfo->NumberOfMessages());
}

CecInbox::CecInbox(u32 id) : boxId(id), messages() {
    u32 inboxInfoSize = 0;
    Result res = CECDU_Open(id, CEC_PATH_INBOX_INFO, CEC_READ, &inboxInfoSize);
    if (R_FAILED(res)) {
        printf("Inbox BoxInfo_____ Open failed: %" PRIX32 "\n", res);
        boxInfo = std::make_unique<BoxInfo>();
    } else {
        std::vector<u8> inboxInfoBuffer(inboxInfoSize);
        res = CECDU_OpenAndRead(inboxInfoSize, id, CEC_PATH_INBOX_INFO, CEC_READ | CEC_CHECK,
                                inboxInfoBuffer.data(), nullptr);
        if (R_FAILED(res)) {
            printf("Inbox BoxInfo_____ OpenAndRead failed: %" PRIX32 "\n", res);
            boxInfo = std::make_unique<BoxInfo>();
        } else {
            boxInfo = std::make_unique<BoxInfo>(inboxInfoBuffer);
        }
    }
}

CecInbox::~CecInbox() {
    printf("CecInbox destructor\n");
}

Result CecInbox::AddMessage(const Message& message) {
    const auto& messageHeader = message.MessageHeader();
    const CecMessageId messageId = message.MessageId();
    const u32 messageSize = message.MessageSize();

    Result res = CECDU_WriteMessage(boxId, false, sizeof(CecMessageId), messageSize,
                                    message.data().data(), messageId.data);
    if (R_FAILED(res)) {
        printf("AddMessage WriteMessage failed: %" PRIX32 "\n", res);
        return res;
    }

    boxInfo->AddMessageHeader(messageHeader);
    return 0;
}

Result CecInbox::DeleteMessage(const CecMessageId& messageId) {
    Result res = CECDU_Delete(boxId, CEC_PATH_INBOX_MSG, false, sizeof(CecMessageId),
                              messageId.data);
    if (R_FAILED(res)) {
        printf("Message Delete failed: %" PRIX32 "\n", res);
        return res;
    }

    boxInfo->DeleteMessageHeader(messageId);
    return 0;
}

Result CecInbox::DeleteAllMessages() {
    
    boxInfo->DeleteAllMessageHeaders();
    return 0;
}

BoxInfo& CecInbox::Info() {
    return *boxInfo;
}

const BoxInfo& CecInbox::Info() const {
    return *boxInfo;
}

std::vector<Message> CecInbox::Messages() {
    return messages;
}

const std::vector<Message> CecInbox::Messages() const {
    return messages;
}

} // namespace Streetpass
