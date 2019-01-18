#include "tests/functions/Write.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Write::Write(const std::string& filepath) : TestBase(filepath) {

}

Write::~Write() {
    log.close();
}

void Write::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
