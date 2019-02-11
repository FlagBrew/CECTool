#include <3ds.h>

#include "common/util.hpp"
#include "delete.hpp"

void displayDeleteMenu(Streetpass::StreetpassManager& sm) {
    consoleClear();
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\nDelete Menu\n\n");
    printf("[A] Delete a Box\n");
    printf("[B] Delete all boxes\n\n");
    printf("Press START for Main Menu\n\n");
}

void displayDeleteSlotSelection(Streetpass::StreetpassManager& sm, u8 slotNum) {
    consoleClear();
    printf("CECTool\n\n");
    sm.ListBoxes();
    printf("\n\n");
    printf("Delete Menu\n\n");
    printf("[A] Select a Box to Delete\n\n");
    printf("Press START for Main Menu\n\n");
    printf("Slot Number: [%x]\n\n", slotNum);
}

void deleteMenu(Streetpass::StreetpassManager& sm) {
    u8 slotNum = 0;
    displayDeleteMenu(sm);
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START)) {
        down = hidKeysDown();
        hidScanInput();

        if (down & KEY_A) {
            displayDeleteSlotSelection(sm, slotNum);
            while (aptMainLoop() && !(down & KEY_START)) {
                down = hidKeysDown();
                hidScanInput();
                if (down & KEY_A) {
                    deleteBox(sm, slotNum);
                    waitForInput();
                    break;
                } else if (down & KEY_DOWN) {
                    if (slotNum > 0) {
                        slotNum--;
                        displayDeleteSlotSelection(sm, slotNum);
                    }
                } else if (down & KEY_UP) {
                    if (slotNum < sm.BoxList().MaxNumberOfSlots() - 1) {
                        slotNum++;
                        displayDeleteSlotSelection(sm, slotNum);
                    }
                }
            }
            break;
        } else if (down & KEY_B) {
            deleteAllBoxes(sm);
            waitForInput();
            break;
        }
    }
}

void deleteBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    printf("This will delete the box in slot [%x].\n", slotNum);
    printf("Are you sure?\n");
    printf("A: Yes\t B: No\n");
    hidScanInput();
    while (aptMainLoop() && !(hidKeysDown() & KEY_A) && !(hidKeysDown() & KEY_B)) {
        hidScanInput();
        if (hidKeysDown() & KEY_A)
        {
            sm.DeleteBox(slotNum);
            printf("Box deleted.\n");
            waitForInput();
            break;
        }
        if (hidKeysDown() & KEY_B)
        {
            printf("Canceled.\n");
            waitForInput();
            break;
        }
    }
}

void deleteAllBoxes(Streetpass::StreetpassManager& sm) {
    printf("This will delete all boxes.\n");
    printf("Are you sure?\n");
    printf("A: Yes\t B: No\n");
    hidScanInput();
    while (aptMainLoop() && !(hidKeysDown() & KEY_A) && !(hidKeysDown() & KEY_B)) {
        hidScanInput();
        if (hidKeysDown() & KEY_A)
        {
            sm.DeleteAllBoxes();
            printf("All boxes deleted.\n");
            waitForInput();
            break;
        }
        if (hidKeysDown() & KEY_B)
        {
            printf("Canceled.\n");
            waitForInput();
            break;
        }
    }
}
