#include "tests/functions/Open.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Open::Open(const std::string& filepath) : TestBase(filepath) {

}

Open::~Open() {
    log.close();
}

void Open::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
