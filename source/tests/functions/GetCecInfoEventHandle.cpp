#include "tests/functions/GetCecInfoEventHandle.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetCecInfoEventHandle::GetCecInfoEventHandle(const std::string& filepath) : TestBase(filepath) {

}

GetCecInfoEventHandle::~GetCecInfoEventHandle() {
    log.close();
}

void GetCecInfoEventHandle::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
