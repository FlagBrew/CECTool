#include "tests/functions/GetEventLog.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetEventLog::GetEventLog(const std::string& filepath) : TestBase(filepath) {

}

GetEventLog::~GetEventLog() {
    log.close();
}

void GetEventLog::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
