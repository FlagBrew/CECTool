#include "tests/functions/GetCecdState.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetCecdState::GetCecdState(const std::string& filepath) : TestBase(filepath) {

}

GetCecdState::~GetCecdState() {
    log.close();
}

void GetCecdState::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
