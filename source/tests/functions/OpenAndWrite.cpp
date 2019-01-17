#include "tests/functions/OpenAndWrite.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

OpenAndWrite::OpenAndWrite(const std::string& filepath) : TestBase(filepath) {

}

OpenAndWrite::~OpenAndWrite() {
    log.close();
}

void OpenAndWrite::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
