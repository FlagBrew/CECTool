#include "tests/functions/ReadData.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

ReadData::ReadData(const std::string& filepath) : TestBase(filepath) {

}

ReadData::~ReadData() {
    log.close();
}

void ReadData::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
