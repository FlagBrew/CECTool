#include "tests/functions/ReadMessageWithHMAC.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

ReadMessageWithHMAC::ReadMessageWithHMAC(const std::string& filepath) : TestBase(filepath) {

}

ReadMessageWithHMAC::~ReadMessageWithHMAC() {
    log.close();
}

void ReadMessageWithHMAC::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
