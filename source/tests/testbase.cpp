#include "tests/testbase.hpp"

namespace Tests {

TestBase::TestBase(const std::string& filepath) : logpath(filepath) {
    log.open(filepath, std::fstream::out | std::fstream::trunc);
}

TestBase::~TestBase() {
    log.close();
}

void TestBase::RunTest() {

}

} // namespace Tests