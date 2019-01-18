#include <3ds.h>
#include <inttypes.h>
#include "common/util.hpp"
#include "create.hpp"
#include "export.hpp"
#include "import.hpp"
#include "tests/tests.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

int __stacksize__ = 64 * 1024;

int main()
{
    Result res;
    gfxInitDefault();
    hidInit();
    hidScanInput();
    sdmcInit();
    consoleInit(GFX_TOP, nullptr);
    if (R_FAILED(res = cecduInit())) printf("Init: %" PRIX32 "\n", res);
    bool showMenu = true;
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START))
    {
        if (showMenu)
        {
            consoleClear();
            printf("What to do?\n");
            printf("A: export all boxes\n");
            printf("B: import all boxes\n");
            printf("Y: create SSB box\n");
            // Possible: \nX: import and delete all SD boxes
            printf("Press START to exit\n");
            showMenu = false;
        }
        down = hidKeysDown();
        hidScanInput();
        if (down & KEY_A)
        {
            printf("Exporting...\n");
            dumpBoxes();
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        else if (down & KEY_B)
        {
            printf("Importing...\n");
            importBoxes(false);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        else if (down & KEY_Y)
        {
            printf("Creating...\n");
            createBox("000b8b00");
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        // Not right now
        // else if (down & KEY_X)
        // {
        //     printf("Importing...\n");
        //     importBoxes(true);
        //     printf("Done!\n");
        //     waitForInput();
        //     showMenu = true;
        // }
        else if (down & KEY_L) {
            printf("Running tests...\n");
            Tests::RunAllTests();
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
    }
    cecduExit();
    return 0;
}
