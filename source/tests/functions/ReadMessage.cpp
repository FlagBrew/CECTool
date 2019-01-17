#include "tests/functions/ReadMessage.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

namespace Tests {

ReadMessage::ReadMessage(const std::string& filepath) : TestBase(filepath) {

}

ReadMessage::~ReadMessage() {
    log.close();
}

void ReadMessage::RunTest() {
    log << "Starting Test" << std::endl;
    log << "Test Finished" << std::endl;
}

} // namespace Tests
