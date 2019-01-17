#include "tests/functions/GetChangeStateEventHandle.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetChangeStateEventHandle::GetChangeStateEventHandle(const std::string& filepath) : TestBase(filepath) {

}

GetChangeStateEventHandle::~GetChangeStateEventHandle() {
    log.close();
}

void GetChangeStateEventHandle::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
