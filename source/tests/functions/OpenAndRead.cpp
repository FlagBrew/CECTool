#include "tests/functions/OpenAndRead.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

OpenAndRead::OpenAndRead(const std::string& filepath) : TestBase(filepath) {

}

OpenAndRead::~OpenAndRead() {
    log.close();
}

void OpenAndRead::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
