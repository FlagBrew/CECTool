#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetEventLog : public TestBase {
public:
    explicit GetEventLog(const std::string& filepath);
    ~GetEventLog() override;

    void RunTest() override;
private:

};

} // namespace Tests
