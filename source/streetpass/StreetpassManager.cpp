#include "streetpass/StreetpassManager.hpp"
#include <fstream>
#include <sys/stat.h>
#include "base64.hpp"
#include <algorithm>
#include "STDirectory.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

StreetpassManager::StreetpassManager() {
    Result res = ReloadBoxList();
    if (R_FAILED(res)) {
        mboxList = std::make_unique<MBoxList>();
    }
}

StreetpassManager::~StreetpassManager() {}

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
        if (slotNum % 3 == 2) {
            printf("\n");
        }
    }

    return 0;
}

Result StreetpassManager::CreateBox(u32 boxId, const std::string& boxName, std::unique_ptr<MBox> mbox) {
    //MBoxInfo
    //MBoxData.001
    //MBoxData.010
    //MBoxData.050
    //InBox/BoxInfo
    //OutBox/BoxInfo
    //OutBox/OBIndex
    //update mboxlist
    Result res = CECDU_Open(boxId, CEC_PATH_MBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("MBox Open Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(sizeof(CecMBoxInfoHeader), boxId, CEC_PATH_MBOX_INFO,
                             CEC_WRITE | CEC_CREATE, mbox->data().data());
    if (R_FAILED(res)) {
        printf("MBoxInfo OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->BoxIcon().size(), boxId, CEC_MBOX_ICON,
                             CEC_WRITE | CEC_CREATE, mbox->BoxIcon().data().data());
    if (R_FAILED(res)) {
        printf("MBoxIcon OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->BoxTitle().size(), boxId, CEC_MBOX_TITLE,
                             CEC_WRITE | CEC_CREATE, mbox->BoxTitle().data().data());
    if (R_FAILED(res)) {
        printf("MBoxTitle OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->BoxProgramId().size(), boxId, CEC_MBOX_PROGRAM_ID,
                             CEC_WRITE | CEC_CREATE, mbox->BoxProgramId().data().data());
    if (R_FAILED(res)) {
        printf("MBoxProgramId OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    res = CECDU_Open(boxId, CEC_PATH_INBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("Inbox Open Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->Inbox().Info().FileSize(), boxId, CEC_PATH_INBOX_INFO,
                             CEC_WRITE | CEC_CREATE, mbox->Inbox().Info().data().data());
    if (R_FAILED(res)) {
        printf("Inbox BoxInfo OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    for (auto message : mbox->Inbox().Messages()) {
        res = CECDU_WriteMessage(boxId, false, sizeof(CecMessageId), message.MessageSize(),
                                message.data().data(), message.MessageId().data);
        if (R_FAILED(res)) {
            printf("Inbox WriteMessage Failed: %lX\n", res);
            return res;
        }
    }

    res = CECDU_Open(boxId, CEC_PATH_OUTBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("Outbox Open Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->Outbox().Info().FileSize(), boxId, CEC_PATH_OUTBOX_INFO,
                             CEC_WRITE | CEC_CREATE, mbox->Outbox().Info().data().data());
    if (R_FAILED(res)) {
        printf("Outbox BoxInfo OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    res = CECDU_OpenAndWrite(mbox->Outbox().Index().FileSize(), boxId, CEC_PATH_OUTBOX_INDEX,
                             CEC_WRITE | CEC_CREATE, mbox->Outbox().Index().data().data());
    if (R_FAILED(res)) {
        printf("Outbox BoxInfo OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    for (auto message : mbox->Outbox().Messages()) {
        res = CECDU_WriteMessage(boxId, true, sizeof(CecMessageId), message.MessageSize(),
                                message.data().data(), message.MessageId().data);
        if (R_FAILED(res)) {
            printf("Outbox WriteMessage Failed: %lX\n", res);
            return res;
        }
    }

    mboxList->AddBox(boxName);

    res = CECDU_OpenAndWrite(sizeof(CecMBoxListHeader), boxId, CEC_PATH_MBOX_LIST,
                             CEC_WRITE, mboxList->data().data());
    if (R_FAILED(res)) {
        printf("MBoxList OpenAndWrite Failed: %lX\n", res);
        return res;
    }

    ReloadBoxList();

    return 0;
}

Result StreetpassManager::DeleteBox(u8 slotNum) {
    if (slotNum < 0 || slotNum > 11) {
        printf("DeleteBox: Slot Number out of range\n");
        return 1;
    }
    std::vector<u32> boxIds = mboxList->BoxIds();
    u32 id = boxIds[slotNum];

    if (id == 0) {
        return 1;
    }

    Result res = CECDU_Delete(id, CEC_PATH_MBOX_DIR, false, 0, nullptr);
    if (R_FAILED(res)) {
        printf("DeleteBox Delete MBox Dir Failed: %lX\n", res);
        return 1;
    }

    res = mboxList->DeleteBox(slotNum);
    if (R_FAILED(res)) {
        printf("MBoxList RemoveBox(slotNum) Failed: %lX\n", res);
        return 1;
    }

    res = CECDU_OpenAndWrite(sizeof(CecMBoxListHeader), 0x0, CEC_PATH_MBOX_LIST, CEC_WRITE,
                             mboxList->data().data());
    if (R_FAILED(res)) {
        printf("MBoxList OpenAndWrite Failed: %lX\n", res);
        return 1;
    }

    ReloadBoxList();

    return 0;
}

Result StreetpassManager::DeleteAllBoxes() {
    for (u32 boxId : mboxList->BoxIds()) {
        if (boxId != 0) {
            Result res = CECDU_Delete(boxId, CEC_PATH_MBOX_DIR, false, 0, nullptr);
            if (R_FAILED(res)) {
                printf("DeleteAllBoxes Delete MBox Dir Failed: %lX\n", res);
            }
        }
    }

    mboxList->DeleteAllBoxes();

    Result res = CECDU_OpenAndWrite(sizeof(CecMBoxListHeader), 0x0, CEC_PATH_MBOX_LIST, CEC_WRITE,
                             mboxList->data().data());
    if (R_FAILED(res)) {
        printf("MBoxList OpenAndWrite Failed: %lX\n", res);
        return 1;
    }

    ReloadBoxList();

    return 0;
}

std::shared_ptr<MBox> StreetpassManager::OpenBox(u8 slotNum) const {
    if (slotNum < 0 || slotNum > 11) {
        printf("OpenBox: Slot Number out of range\n");
        return nullptr;
    }
    if (boxes[slotNum])
    {
        return boxes[slotNum];
    }
    std::vector<u32> boxIds = mboxList->BoxIds();
    u32 id = boxIds[slotNum];

    if (id == 0) {
        return nullptr;
    }

    u32 mboxInfoSize = 0;
    Result res = CECDU_Open(id, CEC_PATH_MBOX_INFO, CEC_READ, &mboxInfoSize);
    if (R_FAILED(res)) {
        printf("MBoxInfo Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> mboxInfoHeaderBuffer(mboxInfoSize);
    res = CECDU_Read(mboxInfoSize, mboxInfoHeaderBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxInfo Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 mboxIconSize = 0;
    res = CECDU_Open(id, CEC_MBOX_ICON, CEC_READ, &mboxIconSize);
    if (R_FAILED(res)) {
        printf("MBoxIcon Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> mboxIconBuffer(mboxIconSize);
    res = CECDU_Read(mboxIconSize, mboxIconBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxIcon Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 mboxProgramIdSize = 0;
    res = CECDU_Open(id, CEC_MBOX_PROGRAM_ID, CEC_READ, &mboxProgramIdSize);
    if (R_FAILED(res)) {
        printf("MBoxProgramId Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> mboxProgramIdBuffer(mboxProgramIdSize);
    res = CECDU_Read(mboxProgramIdSize, mboxProgramIdBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxProgramId Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 mboxTitleSize = 0;
    res = CECDU_Open(id, CEC_MBOX_TITLE, CEC_READ, &mboxTitleSize);
    if (R_FAILED(res)) {
        printf("MBoxTitle Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> mboxTitleBuffer(mboxTitleSize);
    res = CECDU_Read(mboxTitleSize, mboxTitleBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxTitle Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 inboxInfoSize = 0;
    res = CECDU_Open(id, CEC_PATH_INBOX_INFO, CEC_READ, &inboxInfoSize);
    if (R_FAILED(res)) {
        printf("InboxInfo Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> inboxInfoBuffer(inboxInfoSize);
    res = CECDU_Read(inboxInfoSize, inboxInfoBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("InboxInfo Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 outboxInfoSize = 0;
    res = CECDU_Open(id, CEC_PATH_OUTBOX_INFO, CEC_READ, &outboxInfoSize);
    if (R_FAILED(res)) {
        printf("OutboxInfo Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> outboxInfoBuffer(outboxInfoSize);
    res = CECDU_Read(outboxInfoSize, outboxInfoBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("OutboxInfo Read Failed: %lX\n", res);
        return nullptr;
    }

    u32 obIndexSize = 0;
    res = CECDU_Open(id, CEC_PATH_OUTBOX_INDEX, CEC_READ, &obIndexSize);
    if (R_FAILED(res)) {
        printf("OBIndex Open Failed: %lX\n", res);
        return nullptr;
    }
    std::vector<u8> obIndexBuffer(obIndexSize);
    res = CECDU_Read(obIndexSize, obIndexBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("OBIndex Read Failed: %lX\n", res);
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

    return boxes[slotNum] = std::make_shared<MBox>(std::move(inbox), std::move(outbox), std::move(icon),
        std::move(programId), std::move(title), mboxInfoHeaderBuffer);
}

MBoxList& StreetpassManager::BoxList() {
    return *mboxList;
}

const MBoxList& StreetpassManager::BoxList() const {
    return *mboxList;
}

Result StreetpassManager::ReloadBoxList() {
    std::vector<u8> mboxListBuffer(sizeof(CecMBoxListHeader));
    Result res = CECDU_OpenAndRead(sizeof(CecMBoxListHeader), 0x0, CEC_PATH_MBOX_LIST, CEC_READ,
                                   mboxListBuffer.data(), nullptr);
    if (R_FAILED(res)) {
        printf("MBoxList OpenAndRead Failed: %lX\n", res);
        return 1;
    } else {
        mboxList = std::make_unique<MBoxList>(mboxListBuffer);
    }
    return 0;
}

void StreetpassManager::BackupBox(u8 slotNum)
{
    const std::string boxName = BoxList().BoxNames()[slotNum];
    std::shared_ptr<Streetpass::MBox> mbox = OpenBox(slotNum);
    if (mbox) {
        const std::string mboxExportPath = "/3ds/CECTool/export/" + boxName + "/";
        const std::string mboxExportInboxPath = mboxExportPath + "InBox___/";
        const std::string mboxExportOutboxPath = mboxExportPath + "OutBox__/";
        mkdir(mboxExportPath.c_str(), 777);
        mkdir(mboxExportInboxPath.c_str(), 777);
        mkdir(mboxExportOutboxPath.c_str(), 777);

        std::ofstream out(mboxExportPath + "MBoxInfo____", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->data().data()), sizeof(Streetpass::CecMBoxInfoHeader));
        out.close();

        out.open(mboxExportPath + "MBoxData.001", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->BoxIcon().data().data()), mbox->BoxIcon().size());
        out.close();

        out.open(mboxExportPath + "MBoxData.010", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->BoxTitle().data().data()), mbox->BoxTitle().size());
        out.close();

        out.open(mboxExportPath + "MBoxData.050", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->BoxProgramId().data().data()), mbox->BoxProgramId().size());
        out.close();

        out.open(mboxExportInboxPath + "BoxInfo_____", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->Inbox().Info().data().data()), mbox->Inbox().Info().FileSize());
        out.close();

        for (auto message : mbox->Inbox().Messages()) {
            std::string fileName = base64_encode(reinterpret_cast<const char*>(message.MessageId().data), sizeof(CecMessageId));
            out.open(mboxExportInboxPath + fileName, std::ios::out | std::ios::binary | std::ios::trunc);
            out.write(reinterpret_cast<const char*>(message.data().data()), message.MessageSize());
            out.close();
        }

        out.open(mboxExportOutboxPath + "BoxInfo_____", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->Outbox().Info().data().data()), mbox->Outbox().Info().FileSize());
        out.close();

        out.open(mboxExportOutboxPath + "OBIndex_____", std::ios::out | std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(mbox->Outbox().Index().data().data()), mbox->Outbox().Index().FileSize());
        out.close();

        for (auto message : mbox->Outbox().Messages()) {
            std::string fileName = base64_encode(reinterpret_cast<const char*>(message.MessageId().data), sizeof(CecMessageId));
            out.open(mboxExportOutboxPath + fileName, std::ios::out | std::ios::binary | std::ios::trunc);
            out.write(reinterpret_cast<const char*>(message.data().data()), message.MessageSize());
            out.close();
        }
    }
}

Result StreetpassManager::ImportBox(u32 boxId)
{
    // check if box already exists...
    std::vector<u32> currentBoxIds = BoxList().BoxIds();

    if(std::any_of(currentBoxIds.cbegin(), currentBoxIds.cend(), [&boxId](u32 id){ return id == boxId;})) {
        printf("Box already exists.\n");
        return 0;
    }
    char boxName[9] = {0};
    sprintf(boxName, "%08lx", boxId);
    const std::string mboxExportPath = "/3ds/CECTool/export/" + std::string(boxName) + "/";
    const std::string mboxExportInboxPath = mboxExportPath + "InBox___/";
    const std::string mboxExportOutboxPath = mboxExportPath + "OutBox__/";

    std::ifstream in(mboxExportPath + "MBoxInfo____", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 mboxInfoSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> mboxInfoBuffer(mboxInfoSize);
    in.read(reinterpret_cast<char*>(mboxInfoBuffer.data()), mboxInfoSize);
    in.close();

    in.open(mboxExportPath + "MBoxData.001", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 mboxIconSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> mboxIconBuffer(mboxIconSize);
    in.read(reinterpret_cast<char*>(mboxIconBuffer.data()), mboxIconSize);
    in.close();

    in.open(mboxExportPath + "MBoxData.010", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 mboxTitleSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> mboxTitleBuffer(mboxTitleSize);
    in.read(reinterpret_cast<char*>(mboxTitleBuffer.data()), mboxTitleSize);
    in.close();

    in.open(mboxExportPath + "MBoxData.050", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 mboxProgramIdSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> mboxProgramIdBuffer(mboxProgramIdSize);
    in.read(reinterpret_cast<char*>(mboxProgramIdBuffer.data()), mboxProgramIdSize);
    in.close();

    in.open(mboxExportInboxPath + "BoxInfo_____", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 inboxInfoSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> inboxInfoBuffer(inboxInfoSize);
    in.read(reinterpret_cast<char*>(inboxInfoBuffer.data()), inboxInfoSize);
    in.close();

    // inbox messages
    std::unique_ptr<STDirectory> inboxDirectory = std::make_unique<STDirectory>(mboxExportInboxPath);
    std::vector<Streetpass::Message> inboxMessages{};
    const std::string boxInfoString = "BoxInfo_____";
    for (u8 messageNum = 0; messageNum < inboxDirectory->count(); messageNum++) {
        const std::string messageName = inboxDirectory->item(messageNum);
        if (messageName != boxInfoString) {
            in.open(mboxExportInboxPath + messageName, std::ios::in | std::ios::binary);
            in.seekg(0, in.end);
            const u32 messageSize = in.tellg();
            in.seekg(0, in.beg);

            std::vector<u8> messageBuffer(messageSize);
            in.read(reinterpret_cast<char*>(messageBuffer.data()), messageSize);
            in.close();

            inboxMessages.emplace_back(messageBuffer);
        }
    }

    in.open(mboxExportOutboxPath + "BoxInfo_____", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 outboxInfoSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> outboxInfoBuffer(outboxInfoSize);
    in.read(reinterpret_cast<char*>(outboxInfoBuffer.data()), outboxInfoSize);
    in.close();

    in.open(mboxExportOutboxPath + "OBIndex_____", std::ios::in | std::ios::binary);
    in.seekg(0, in.end);
    const u32 obIndexSize = in.tellg();
    in.seekg(0, in.beg);
    std::vector<u8> obIndexBuffer(obIndexSize);
    in.read(reinterpret_cast<char*>(obIndexBuffer.data()), obIndexSize);
    in.close();

    // outbox messages
    std::unique_ptr<STDirectory> outboxDirectory = std::make_unique<STDirectory>(mboxExportOutboxPath);
    std::vector<Streetpass::Message> outboxMessages{};
    const std::string obIndexString = "OBIndex_____";
    for (u8 messageNum = 0; messageNum < outboxDirectory->count(); messageNum++) {
        const std::string messageName = outboxDirectory->item(messageNum);
        if (messageName != boxInfoString && messageName.c_str() != obIndexString) {
            in.open(mboxExportOutboxPath + messageName, std::ios::in | std::ios::binary);
            in.seekg(0, in.end);
            const u32 messageSize = in.tellg();
            in.seekg(0, in.beg);
            
            std::vector<u8> messageBuffer(messageSize);
            in.read(reinterpret_cast<char*>(messageBuffer.data()), messageSize);
            in.close();

            outboxMessages.emplace_back(messageBuffer);
        }
    }

    std::unique_ptr<Streetpass::BoxInfo> inboxInfo =
        std::make_unique<Streetpass::BoxInfo>(inboxInfoBuffer);
    std::unique_ptr<Streetpass::BoxInfo> outboxInfo =
        std::make_unique<Streetpass::BoxInfo>(outboxInfoBuffer);
    std::unique_ptr<Streetpass::OBIndex> obIndex =
        std::make_unique<Streetpass::OBIndex>(obIndexBuffer);

    std::unique_ptr<Streetpass::CecInbox> inbox =
        std::make_unique<Streetpass::CecInbox>(boxId, std::move(inboxInfo), inboxMessages);
    std::unique_ptr<Streetpass::CecOutbox> outbox =
        std::make_unique<Streetpass::CecOutbox>(boxId, std::move(outboxInfo), std::move(obIndex), outboxMessages);

    std::unique_ptr<Streetpass::MBoxIcon> icon =
        std::make_unique<Streetpass::MBoxIcon>(mboxIconBuffer);
    std::unique_ptr<Streetpass::MBoxProgramId> programId =
        std::make_unique<Streetpass::MBoxProgramId>(mboxProgramIdBuffer);
    std::unique_ptr<Streetpass::MBoxTitle> title =
        std::make_unique<Streetpass::MBoxTitle>(mboxTitleBuffer);

    std::unique_ptr<Streetpass::MBox> mbox =
        std::make_unique<Streetpass::MBox>(std::move(inbox), std::move(outbox), std::move(icon),
        std::move(programId), std::move(title), mboxInfoBuffer);
    
    return CreateBox(boxId, boxName, std::move(mbox));
}

} // namespace Streetpass
