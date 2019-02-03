#include <3ds.h>

#include "delete.hpp"
#include "common/util.hpp"

void deleteBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    sm.DeleteBox(slotNum);
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
            break;
        }
        if (hidKeysDown() & KEY_B)
        {
            printf("Canceled\n");
            break;
        }
    }
    waitForInput();
}
