#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class EventLogTest : public TestBase {
public:
    explicit EventLogTest(const std::string& filepath);
    ~EventLogTest() override;

    void RunTest() override;
private:

};

} // namespace Tests
