#include <3ds.h>
#include "common/util.hpp"

void waitForInput()
{
    while (aptMainLoop() && !hidKeysDown()) hidScanInput();
    hidScanInput();
}
