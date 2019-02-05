#include <3ds.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <memory>

#include "common/io.hpp"
#include "common/STDirectory.hpp"
#include "common/util.hpp"
#include "create.hpp"
#include "streetpass/BoxInfo.hpp"
#include "streetpass/CecInbox.hpp"
#include "streetpass/CecOutbox.hpp"
#include "streetpass/MBox.hpp"
#include "streetpass/MBoxIcon.hpp"
#include "streetpass/MBoxProgramId.hpp"
#include "streetpass/MBoxTitle.hpp"
#include "streetpass/Message.hpp"
#include "streetpass/OBIndex.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

void displayCreateMenu(Streetpass::StreetpassManager& sm) {
    consoleClear();
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nCreate Menu\n\n");
    printf("[A] Create Box from Exported Backup\n\n");
    //printf("[B] Create New Box\n"); // Need to figure out common defaults
    printf("Press START for Main Menu\n\n");
}

void displayBackupSlotSelection(Streetpass::StreetpassManager& sm, STDirectory& backupDirectory, u8 slotNum) {
    consoleClear();
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nCreate Menu\n\n");
    printf("[A] Select a Box\n\n");
    printf("Press START for Main Menu\n\n");
    for(u8 slotNum = 0; slotNum < backupDirectory.count(); slotNum++) {
        printf("[%x] %s  ", slotNum, backupDirectory.item(slotNum).c_str());
        if (slotNum % 3 == 2) {
            printf("\n");
        }
    }
    printf("\n\nSlot Number: [%x]\n\n", slotNum);
}

void createMenu(Streetpass::StreetpassManager& sm) {
    u8 slotNum = 0;
    displayCreateMenu(sm);
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START)) {
        down = hidKeysDown();
        hidScanInput();

        if(down & KEY_A) {
            if(sm.BoxList().NumberOfSlotsUsed() >= sm.BoxList().MaxNumberOfSlots()) {
                printf("Box list is full. Please delete a box and try again.\n");
                waitForInput();
                break;
            }
            std::unique_ptr<STDirectory> backupDirectory =
                std::make_unique<STDirectory>("/3ds/CECTool/export/");
            displayBackupSlotSelection(sm, *backupDirectory, slotNum);
            while (aptMainLoop() && !(down & KEY_START)) {
                down = hidKeysDown();
                hidScanInput();
                if (down & KEY_A) {
                    createBoxFromBackup(sm, *backupDirectory, slotNum);
                    waitForInput();
                    break;
                } else if (down & KEY_DOWN) {
                    if (slotNum > 0) {
                        slotNum--;
                        displayBackupSlotSelection(sm, *backupDirectory, slotNum);
                    }
                } else if (down & KEY_UP) {
                    if (slotNum < backupDirectory->count() - 1) {
                        slotNum++;
                        displayBackupSlotSelection(sm, *backupDirectory, slotNum);
                    }
                }
            }
            break;
        }
    }
}

void createBoxFromBackup(Streetpass::StreetpassManager& sm, STDirectory& backupDirectory, u8 slotNum) {
    // check if box already exists...
    const u32 boxId = std::stoul(backupDirectory.item(slotNum), nullptr, 16);
    std::vector<u32> currentBoxIds = sm.BoxList().BoxIds();

    if(std::any_of(currentBoxIds.cbegin(), currentBoxIds.cend(), [&boxId](u32 id){ return id == boxId;})) {
        printf("Box already exists.\n");
        return;
    }

    const std::string boxName = backupDirectory.item(slotNum);
    const std::string mboxExportPath = "/3ds/CECTool/export/" + boxName + "/";
    const std::string mboxExportInboxPath = mboxExportPath + "InBox___/";
    const std::string mboxExportOutboxPath = mboxExportPath + "OutBox__/";

    std::ifstream mboxInfo(mboxExportPath + "MBoxInfo____", std::ios::in | std::ios::binary);
    mboxInfo.seekg(0, mboxInfo.end);
    const u32 mboxInfoSize = mboxInfo.tellg();
    mboxInfo.seekg(0, mboxInfo.beg);
    std::vector<u8> mboxInfoBuffer(mboxInfoSize);
    mboxInfo.read(reinterpret_cast<char*>(mboxInfoBuffer.data()), mboxInfoSize);
    mboxInfo.close();

    std::ifstream mboxIcon(mboxExportPath + "MBoxData.001", std::ios::in | std::ios::binary);
    mboxIcon.seekg(0, mboxIcon.end);
    const u32 mboxIconSize = mboxIcon.tellg();
    mboxIcon.seekg(0, mboxIcon.beg);
    std::vector<u8> mboxIconBuffer(mboxIconSize);
    mboxIcon.read(reinterpret_cast<char*>(mboxIconBuffer.data()), mboxIconSize);
    mboxIcon.close();

    std::ifstream mboxTitle(mboxExportPath + "MBoxData.010", std::ios::in | std::ios::binary);
    mboxTitle.seekg(0, mboxTitle.end);
    const u32 mboxTitleSize = mboxTitle.tellg();
    mboxTitle.seekg(0, mboxTitle.beg);
    std::vector<u8> mboxTitleBuffer(mboxTitleSize);
    mboxTitle.read(reinterpret_cast<char*>(mboxTitleBuffer.data()), mboxTitleSize);
    mboxTitle.close();

    std::ifstream mboxProgramId(mboxExportPath + "MBoxData.050", std::ios::in | std::ios::binary);
    mboxProgramId.seekg(0, mboxProgramId.end);
    const u32 mboxProgramIdSize = mboxProgramId.tellg();
    mboxProgramId.seekg(0, mboxProgramId.beg);
    std::vector<u8> mboxProgramIdBuffer(mboxProgramIdSize);
    mboxProgramId.read(reinterpret_cast<char*>(mboxProgramIdBuffer.data()), mboxProgramIdSize);
    mboxProgramId.close();

    std::ifstream inboxInfoFile(mboxExportInboxPath + "BoxInfo_____", std::ios::in | std::ios::binary);
    inboxInfoFile.seekg(0, inboxInfoFile.end);
    const u32 inboxInfoSize = inboxInfoFile.tellg();
    inboxInfoFile.seekg(0, inboxInfoFile.beg);
    std::vector<u8> inboxInfoBuffer(inboxInfoSize);
    inboxInfoFile.read(reinterpret_cast<char*>(inboxInfoBuffer.data()), inboxInfoSize);
    inboxInfoFile.close();

    // inbox messages
    std::unique_ptr<STDirectory> inboxDirectory = std::make_unique<STDirectory>(mboxExportInboxPath);
    std::vector<Streetpass::Message> inboxMessages{};
    const std::string boxInfoString = "BoxInfo_____";
    for (u8 messageNum = 0; messageNum < inboxDirectory->count(); messageNum++) {
        const std::string messageName = inboxDirectory->item(messageNum);
        if (messageName != boxInfoString) {
            std::ifstream message(mboxExportInboxPath + messageName, std::ios::in | std::ios::binary);
            message.seekg(0, message.end);
            const u32 messageSize = message.tellg();
            message.seekg(0, message.beg);

            std::vector<u8> messageBuffer(messageSize);
            message.read(reinterpret_cast<char*>(messageBuffer.data()), messageSize);
            message.close();

            inboxMessages.emplace_back(messageBuffer);
        }
    }

    std::ifstream outboxInfoFile(mboxExportOutboxPath + "BoxInfo_____", std::ios::in | std::ios::binary);
    outboxInfoFile.seekg(0, outboxInfoFile.end);
    const u32 outboxInfoSize = outboxInfoFile.tellg();
    outboxInfoFile.seekg(0, outboxInfoFile.beg);
    std::vector<u8> outboxInfoBuffer(outboxInfoSize);
    outboxInfoFile.read(reinterpret_cast<char*>(outboxInfoBuffer.data()), outboxInfoSize);
    outboxInfoFile.close();

    std::ifstream obIndexFile(mboxExportOutboxPath + "OBIndex_____", std::ios::in | std::ios::binary);
    obIndexFile.seekg(0, obIndexFile.end);
    const u32 obIndexSize = obIndexFile.tellg();
    obIndexFile.seekg(0, obIndexFile.beg);
    std::vector<u8> obIndexBuffer(obIndexSize);
    obIndexFile.read(reinterpret_cast<char*>(obIndexBuffer.data()), obIndexSize);
    obIndexFile.close();

    // outbox messages
    std::unique_ptr<STDirectory> outboxDirectory = std::make_unique<STDirectory>(mboxExportOutboxPath);
    std::vector<Streetpass::Message> outboxMessages{};
    const std::string obIndexString = "OBIndex_____";
    for (u8 messageNum = 0; messageNum < outboxDirectory->count(); messageNum++) {
        const std::string messageName = outboxDirectory->item(messageNum);
        if (messageName != boxInfoString && messageName.c_str() != obIndexString) {
            std::ifstream message(mboxExportOutboxPath + messageName, std::ios::in | std::ios::binary);
            message.seekg(0, message.end);
            const u32 messageSize = message.tellg();
            message.seekg(0, message.beg);
            
            std::vector<u8> messageBuffer(messageSize);
            message.read(reinterpret_cast<char*>(messageBuffer.data()), messageSize);
            message.close();

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
    
    sm.CreateBox(boxId, boxName, std::move(mbox));
}
