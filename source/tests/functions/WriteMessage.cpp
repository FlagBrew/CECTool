#include "tests/functions/WriteMessage.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

WriteMessage::WriteMessage(const std::string& filepath) : TestBase(filepath) {

}

WriteMessage::~WriteMessage() {
    log.close();
}

void WriteMessage::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
