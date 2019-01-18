#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class MBoxListTest : public TestBase {
public:
    explicit MBoxListTest(const std::string& filepath);
    ~MBoxListTest() override;

    void RunTest() override;
private:

};

} // namespace Tests
