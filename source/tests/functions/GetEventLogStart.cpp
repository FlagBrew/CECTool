#include "tests/functions/GetEventLogStart.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetEventLogStart::GetEventLogStart(const std::string& filepath) : TestBase(filepath) {

}

GetEventLogStart::~GetEventLogStart() {
    log.close();
}

void GetEventLogStart::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
