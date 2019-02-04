#include <3ds.h>
#include <fstream>
#include <sys/stat.h>
#include "common/base64.hpp"
#include "export.hpp"

#include "streetpass/MBox.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

void exportAllBoxes(Streetpass::StreetpassManager& sm) {
    for (u8 slotNum = 0; slotNum < sm.BoxList().MaxNumberOfSlots(); slotNum++) {
        exportBox(sm, slotNum);
    }
    printf("All boxes exported.\n");
}

void exportBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    const std::string boxName = sm.BoxList().BoxNames()[slotNum];
    std::unique_ptr<Streetpass::MBox> mbox = sm.OpenBox(slotNum);
    if (mbox) {
        //printf("%lx : %lx\n", mbox->ProgramId(), mbox->PrivateId());

        const std::string mboxExportPath = "/3ds/CECTool/export/" + boxName + "/";
        const std::string mboxExportInboxPath = mboxExportPath + "InBox___/";
        const std::string mboxExportOutboxPath = mboxExportPath + "OutBox__/";
        mkdir(mboxExportPath.c_str(), 777);
        mkdir(mboxExportInboxPath.c_str(), 777);
        mkdir(mboxExportOutboxPath.c_str(), 777);

        std::ofstream mboxInfo(mboxExportPath + "MBoxInfo____", std::ios::out | std::ios::binary | std::ios::trunc);
        mboxInfo.write(reinterpret_cast<const char*>(mbox->data().data()), sizeof(Streetpass::CecMBoxInfoHeader));
        mboxInfo.close();

        std::ofstream mboxIcon(mboxExportPath + "MBoxData.001", std::ios::out | std::ios::binary | std::ios::trunc);
        mboxIcon.write(reinterpret_cast<const char*>(mbox->BoxIcon().data().data()), mbox->BoxIcon().size());
        mboxIcon.close();

        std::ofstream mboxTitle(mboxExportPath + "MBoxData.010", std::ios::out | std::ios::binary | std::ios::trunc);
        mboxTitle.write(reinterpret_cast<const char*>(mbox->BoxTitle().data().data()), mbox->BoxTitle().size());
        mboxTitle.close();

        std::ofstream mboxProgramId(mboxExportPath + "MBoxData.050", std::ios::out | std::ios::binary | std::ios::trunc);
        mboxProgramId.write(reinterpret_cast<const char*>(mbox->BoxProgramId().data().data()), mbox->BoxProgramId().size());
        mboxProgramId.close();

        std::ofstream inboxInfo(mboxExportInboxPath + "BoxInfo_____", std::ios::out | std::ios::binary | std::ios::trunc);
        inboxInfo.write(reinterpret_cast<const char*>(mbox->Inbox().Info().data().data()), mbox->Inbox().Info().FileSize());
        inboxInfo.close();

        for (auto message : mbox->Inbox().Messages()) {
            std::string fileName = base64_encode(reinterpret_cast<const char*>(message.MessageId().data), sizeof(CecMessageId));
            std::ofstream file(mboxExportInboxPath + fileName, std::ios::out | std::ios::binary | std::ios::trunc);
            file.write(reinterpret_cast<const char*>(message.data().data()), message.MessageSize());
            file.close();
        }

        std::ofstream outboxInfo(mboxExportOutboxPath + "BoxInfo_____", std::ios::out | std::ios::binary | std::ios::trunc);
        outboxInfo.write(reinterpret_cast<const char*>(mbox->Outbox().Info().data().data()), mbox->Outbox().Info().FileSize());
        outboxInfo.close();

        std::ofstream obIndex(mboxExportOutboxPath + "OBIndex_____", std::ios::out | std::ios::binary | std::ios::trunc);
        obIndex.write(reinterpret_cast<const char*>(mbox->Outbox().Index().data().data()), mbox->Outbox().Index().FileSize());
        obIndex.close();

        for (auto message : mbox->Outbox().Messages()) {
            std::string fileName = base64_encode(reinterpret_cast<const char*>(message.MessageId().data), sizeof(CecMessageId));
            std::ofstream file(mboxExportOutboxPath + fileName, std::ios::out | std::ios::binary | std::ios::trunc);
            file.write(reinterpret_cast<const char*>(message.data().data()), message.MessageSize());
            file.close();
        }

        printf("Box in slot [%x] exported.\n", slotNum);
    }
}

