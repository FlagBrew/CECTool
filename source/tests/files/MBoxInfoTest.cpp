#include "tests/files/MBoxInfoTest.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

MBoxInfoTest::MBoxInfoTest(const std::string& filepath) : TestBase(filepath) {

}

MBoxInfoTest::~MBoxInfoTest() {
    log.close();
}

void MBoxInfoTest::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
