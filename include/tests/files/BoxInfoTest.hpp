#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class BoxInfoTest : public TestBase {
public:
    explicit BoxInfoTest(const std::string& filepath);
    ~BoxInfoTest() override;

    void RunTest() override;
private:

};

} // namespace Tests
