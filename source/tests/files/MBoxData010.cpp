#include "tests/files/MBoxData010.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

MBoxData010::MBoxData010(const std::string& filepath) : TestBase(filepath) {

}

MBoxData010::~MBoxData010() {
    log.close();
}

void MBoxData010::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
