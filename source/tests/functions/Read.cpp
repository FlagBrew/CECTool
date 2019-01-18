#include "tests/functions/Read.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Read::Read(const std::string& filepath) : TestBase(filepath) {

}

Read::~Read() {
    log.close();
}

void Read::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
