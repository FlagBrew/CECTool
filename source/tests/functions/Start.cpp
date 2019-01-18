#include "tests/functions/Start.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Start::Start(const std::string& filepath) : TestBase(filepath) {

}

Start::~Start() {
    log.close();
}

void Start::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
