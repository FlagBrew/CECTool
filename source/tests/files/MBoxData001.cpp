#include "tests/files/MBoxData001.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

MBoxData001::MBoxData001(const std::string& filepath) : TestBase(filepath) {

}

MBoxData001::~MBoxData001() {
    log.close();
}

void MBoxData001::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
