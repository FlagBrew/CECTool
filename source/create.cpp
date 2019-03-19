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
    sm.ImportBox(std::stoul(backupDirectory.item(slotNum), nullptr, 16));
}
