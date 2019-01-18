#include "tests/files/MBoxData050.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

MBoxData050::MBoxData050(const std::string& filepath) : TestBase(filepath) {

}

MBoxData050::~MBoxData050() {
    log.close();
}

void MBoxData050::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
