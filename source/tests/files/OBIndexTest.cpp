#include "tests/files/OBIndexTest.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

OBIndexTest::OBIndexTest(const std::string& filepath) : TestBase(filepath) {

}

OBIndexTest::~OBIndexTest() {
    log.close();
}

void OBIndexTest::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
