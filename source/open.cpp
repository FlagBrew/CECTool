#include <3ds.h>
#include <memory>
#include <string>

#include "common/util.hpp"
#include "open.hpp"
#include "streetpass/MBox.hpp"

void displayOpenMenu(Streetpass::StreetpassManager& sm, u8 slotNum) {
    consoleClear();
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nOpen Menu\n\n");
    printf("[A] Select a Box to Open\n\n");
    printf("Press START for Main Menu\n\n");
    printf("Slot Number: [%x]\n\n", slotNum);
}

void openMenu(Streetpass::StreetpassManager& sm) {
    u8 slotNum = 0;
    u32 down = hidKeysDown();
    displayOpenMenu(sm, slotNum);
    while (aptMainLoop() && !(down & KEY_START)) {
        down = hidKeysDown();
        hidScanInput();

        if (down & KEY_A) {
            openBox(sm, slotNum);
            waitForInput();
            break;
        } else if (down & KEY_DOWN) {
            if (slotNum > 0) {
                slotNum--;
                displayOpenMenu(sm, slotNum);
            }
        } else if (down & KEY_UP) {
            if (slotNum < sm.BoxList().MaxNumberOfSlots() - 1) {
                slotNum++;
                displayOpenMenu(sm, slotNum);
            }
        }
    }
}

void openBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    const std::string boxName = sm.BoxList().BoxNames()[slotNum];
    std::shared_ptr<Streetpass::MBox> mbox = sm.OpenBox(slotNum);

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