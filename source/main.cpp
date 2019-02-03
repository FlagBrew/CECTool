#include <3ds.h>
#include <memory>
#include <sys/stat.h>

#include "common/util.hpp"
#include "create.hpp"
#include "delete.hpp"
#include "export.hpp"
#include "import.hpp"
#include "open.hpp"
#include "tests/tests.hpp"
#include "streetpass/StreetpassManager.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

using Streetpass::StreetpassManager;

int __stacksize__ = 64 * 1024;

void cecToolDirectoryCheck() {
    // What happens if there is no sd card...?
    mkdir("/3ds/CECTool", 777);
    mkdir("/3ds/CECTool/export", 777);
    mkdir("/3ds/CECTool/import", 777);
    mkdir("/3ds/CECTool/template", 777);
    mkdir("/3ds/CECTool/tests", 777);
}

void init() {
    gfxInitDefault();
    hidInit();
    hidScanInput();
    sdmcInit();
    consoleInit(GFX_TOP, nullptr);

    Result res = cecduInit();
    if (R_FAILED(res)) {
        printf("Cecdu Init Failed: %lX\n", res);
    }
}

void shutdown() {
    cecduExit();
}

int main()
{
    init();
    cecToolDirectoryCheck();

    std::unique_ptr<StreetpassManager> sm = std::make_unique<StreetpassManager>();

    bool showMenu = true;
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START))
    {
        if (showMenu) {
            consoleClear();
            printf("CECTool\n\n");
            sm->ListBoxes();
            printf("\n\nMain Menu\n\n");

            printf("[A] Create\n");
            printf("[B] Delete\n");
            printf("[X] Export\n");
            printf("[Y] Import\n");
            printf("[L] Open\n");
            printf("[R] Tests\n");

            printf("\nPress START to exit\n");
            showMenu = false;
        }
        down = hidKeysDown();
        hidScanInput();

        if (down & KEY_A) {
            printf("Creating...\n");
            //createBox(*sm);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        } else if (down & KEY_B) {
            //printf("Deleting...\n");
            deleteAllBoxes(*sm);
            //u8 slotNum = 2;
            //deleteBox(*sm, slotNum);
            //printf("Done!\n");
            waitForInput();
            showMenu = true;
        } else if (down & KEY_X) {
            printf("Exporting...\n");
            exportAllBoxes(*sm);
            //u8 slotNum = 0;
            //exportBox(*sm, slotNum);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        } else if (down & KEY_Y) {
            printf("Importing...\n");
            //u8 slotNum = 2;
            //importBox(*sm, slotNum);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        } else if (down & KEY_L) {
            printf("Opening...\n");
            u8 slotNum = 0;
            openBox(*sm, slotNum);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        } else if (down & KEY_R) {
            printf("Testing...\n");
            Tests::RunAllTests();
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
    }
    shutdown();
    return 0;
}
