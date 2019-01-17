#include "tests/functions/Stop.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Stop::Stop(const std::string& filepath) : TestBase(filepath) {

}

Stop::~Stop() {
    log.close();
}

void Stop::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
