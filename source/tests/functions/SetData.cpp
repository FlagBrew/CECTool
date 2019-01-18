#include "tests/functions/SetData.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

SetData::SetData(const std::string& filepath) : TestBase(filepath) {

}

SetData::~SetData() {
    log.close();
}

void SetData::RunTest() {
    log << "Starting Test" << std::endl;

    Result res;
    u32 bufferSize = 256;
    u8 buffer[bufferSize];

    log << "SetData Option 2 Test Example" << std::endl;
    // Option 2 example
    for (u32 size = 0; size < 128; size++)
    {
        res = CECDU_SetData(0x0010c000, size, 2, &buffer);
        log << "buffer size: " << size << " result: " << std::hex << res << std::dec << std::endl;
    }
    log << "Test Finished" << std::endl;
}

} // namespace Tests
