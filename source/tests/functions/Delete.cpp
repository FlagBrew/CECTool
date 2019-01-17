#include "tests/functions/Delete.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

Delete::Delete(const std::string& filepath) : TestBase(filepath) {

}

Delete::~Delete() {
    log.close();
}

void Delete::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
