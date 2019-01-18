#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Stop : public TestBase {
public:
    explicit Stop(const std::string& filepath);
    ~Stop() override;

    void RunTest() override;
private:

};

} // namespace Tests
