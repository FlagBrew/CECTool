#include "tests/functions/GetCecInfoBuffer.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

GetCecInfoBuffer::GetCecInfoBuffer(const std::string& filepath) : TestBase(filepath) {

}

GetCecInfoBuffer::~GetCecInfoBuffer() {
    log.close();
}

void GetCecInfoBuffer::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
