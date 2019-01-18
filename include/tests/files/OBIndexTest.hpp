#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class OBIndexTest : public TestBase {
public:
    explicit OBIndexTest(const std::string& filepath);
    ~OBIndexTest() override;

    void RunTest() override;
private:

};

} // namespace Tests
