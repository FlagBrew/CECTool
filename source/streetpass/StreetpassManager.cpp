#include <inttypes.h>

#include "streetpass/StreetpassManager.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

StreetpassManager::StreetpassManager() {
    // open mboxList
    std::vector<u8> mboxListBuffer(sizeof(CecMBoxListHeader));
    Result res = CECDU_OpenAndRead(sizeof(CecMBoxListHeader), 0x0, CEC_PATH_MBOX_LIST, CEC_READ,
                                   mboxListBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxList OpenAndRead Failed: %" PRIX32 "\n", res);
    } else {
        mboxList = std::make_unique<MBoxList>(mboxListBuffer);
    }

}

StreetpassManager::~StreetpassManager() {

}

Result StreetpassManager::HexDump(const std::vector<u8>& buffer) const {
    printf("\n");
    for (auto hex : buffer) {
        printf("%x", hex);
    }
    printf("\n\n");

    return 0;
}

Result StreetpassManager::ListBoxes() const {
    std::vector<std::string> boxes = mboxList->BoxNames();

    for (u8 slotNum = 0; slotNum < boxes.size(); slotNum++) {
        printf("[%x] %s  ", slotNum, boxes[slotNum].c_str());
        if (slotNum == 2 || slotNum == 5 || slotNum == 8) {
            printf("\n");
        }
    }

    return 0;
}

std::unique_ptr<MBox> StreetpassManager::OpenBox(u8 slotNum) const {
    if (slotNum < 0 || slotNum > 11) {
        printf("OpenBox: Slot Number out of range\n");
        return nullptr;
    }
    std::vector<u32> boxIds = mboxList->BoxIds();
    u32 id = boxIds[slotNum];

    u32 mboxInfoSize = 0;
    Result res = CECDU_Open(id, CEC_PATH_MBOX_INFO, CEC_READ, &mboxInfoSize);
    if (R_FAILED(res)) {
        printf("MBoxInfo Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> mboxInfoHeaderBuffer(mboxInfoSize);
    res = CECDU_Read(mboxInfoSize, mboxInfoHeaderBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxInfo Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 mboxIconSize = 0;
    res = CECDU_Open(id, CEC_MBOX_ICON, CEC_READ, &mboxIconSize);
    if (R_FAILED(res)) {
        printf("MBoxIcon Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> mboxIconBuffer(mboxIconSize);
    res = CECDU_Read(mboxIconSize, mboxIconBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxIcon Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 mboxProgramIdSize = 0;
    res = CECDU_Open(id, CEC_MBOX_PROGRAM_ID, CEC_READ, &mboxProgramIdSize);
    if (R_FAILED(res)) {
        printf("MBoxProgramId Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> mboxProgramIdBuffer(mboxProgramIdSize);
    res = CECDU_Read(mboxProgramIdSize, mboxProgramIdBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxProgramId Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 mboxTitleSize = 0;
    res = CECDU_Open(id, CEC_MBOX_TITLE, CEC_READ, &mboxTitleSize);
    if (R_FAILED(res)) {
        printf("MBoxTitle Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> mboxTitleBuffer(mboxTitleSize);
    res = CECDU_Read(mboxTitleSize, mboxTitleBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxTitle Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 inboxInfoSize = 0;
    res = CECDU_Open(id, CEC_PATH_INBOX_INFO, CEC_READ, &inboxInfoSize);
    if (R_FAILED(res)) {
        printf("InboxInfo Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> inboxInfoBuffer(inboxInfoSize);
    res = CECDU_Read(inboxInfoSize, inboxInfoBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("InboxInfo Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 outboxInfoSize = 0;
    res = CECDU_Open(id, CEC_PATH_OUTBOX_INFO, CEC_READ, &outboxInfoSize);
    if (R_FAILED(res)) {
        printf("OutboxInfo Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> outboxInfoBuffer(outboxInfoSize);
    res = CECDU_Read(outboxInfoSize, outboxInfoBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("OutboxInfo Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    u32 obIndexSize = 0;
    res = CECDU_Open(id, CEC_PATH_OUTBOX_INDEX, CEC_READ, &obIndexSize);
    if (R_FAILED(res)) {
        printf("OBIndex Open Failed: %" PRIX32 "\n", res);
        return nullptr;
    }
    std::vector<u8> obIndexBuffer(obIndexSize);
    res = CECDU_Read(obIndexSize, obIndexBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("OBIndex Read Failed: %" PRIX32 "\n", res);
        return nullptr;
    }

    std::unique_ptr<BoxInfo> inboxInfo = std::make_unique<BoxInfo>(inboxInfoBuffer);
    std::unique_ptr<BoxInfo> outboxInfo = std::make_unique<BoxInfo>(outboxInfoBuffer);
    std::unique_ptr<OBIndex> obIndex = std::make_unique<OBIndex>(obIndexBuffer);

    std::unique_ptr<CecInbox> inbox = std::make_unique<CecInbox>(id, std::move(inboxInfo));
    std::unique_ptr<CecOutbox> outbox = std::make_unique<CecOutbox>(id, std::move(outboxInfo),
                                                                    std::move(obIndex));

    std::unique_ptr<MBoxIcon> icon = std::make_unique<MBoxIcon>(mboxIconBuffer);
    std::unique_ptr<MBoxProgramId> programId = std::make_unique<MBoxProgramId>(mboxProgramIdBuffer);
    std::unique_ptr<MBoxTitle> title = std::make_unique<MBoxTitle>(mboxTitleBuffer);

    return std::make_unique<MBox>(std::move(inbox), std::move(outbox), std::move(icon),
        std::move(programId), std::move(title), mboxInfoHeaderBuffer);
}

MBoxList& StreetpassManager::BoxList() {
    return *mboxList;
}

const MBoxList& StreetpassManager::BoxList() const {
    return *mboxList;
}


} // namespace Streetpass
