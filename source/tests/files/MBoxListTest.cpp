#include "tests/files/MBoxListTest.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

MBoxListTest::MBoxListTest(const std::string& filepath) : TestBase(filepath) {

}

MBoxListTest::~MBoxListTest() {
    log.close();
}

void MBoxListTest::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
