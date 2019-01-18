#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetEventLogStart : public TestBase {
public:
    explicit GetEventLogStart(const std::string& filepath);
    ~GetEventLogStart() override;

    void RunTest() override;
private:

};

} // namespace Tests
