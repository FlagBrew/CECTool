#include "tests/files/EventLogTest.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

EventLogTest::EventLogTest(const std::string& filepath) : TestBase(filepath) {

}

EventLogTest::~EventLogTest() {
    log.close();
}

void EventLogTest::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
