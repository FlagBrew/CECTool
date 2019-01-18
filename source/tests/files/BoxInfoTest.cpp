#include "tests/files/BoxInfoTest.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

BoxInfoTest::BoxInfoTest(const std::string& filepath) : TestBase(filepath) {

}

BoxInfoTest::~BoxInfoTest() {
    log.close();
}

void BoxInfoTest::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
