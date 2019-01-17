#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class MBoxInfoTest : public TestBase {
public:
    explicit MBoxInfoTest(const std::string& filepath);
    ~MBoxInfoTest() override;

    void RunTest() override;
private:

};

} // namespace Tests
