#include <3ds.h>
#include <memory>
#include <string>

#include "streetpass/MBox.hpp"

#include "open.hpp"

void openBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    const std::string boxName = sm.BoxList().BoxNames()[slotNum];
    std::unique_ptr<Streetpass::MBox> mbox = sm.OpenBox(slotNum);

    const std::string rootCecPath = "/CEC/";
    const std::string mboxPath = rootCecPath + boxName + "/";
    //const std::string mboxInboxPath = mboxPath + "InBox___/";
    //const std::string mboxOutboxPath = mboxPath + "OutBox__/";

    if (mbox) {
        consoleClear();
        printf("[%x] Box Id: %s\n\n", slotNum, boxName.c_str());
        printf("%s\n", mboxPath.c_str());
        printf("  InBox___/\n");
        printf("  OutBox__/\n");
        printf("  MBoxData.001\n");
        printf("  MBoxData.010\n");
        printf("  MBoxData.050\n");
        printf("  MBoxInfo____\n\n\n");

        printf("[Hex Dump of MBoxInfo____]\n");
        sm.HexDump(mbox->data());
        
        printf("[Hex Dump of MBoxData.010]\n");
        sm.HexDump(mbox->BoxTitle().data());

        printf("[Hex Dump of MBoxData.050]\n");
        sm.HexDump(mbox->BoxProgramId().data());
    }
}