#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class MBoxData001 : public TestBase {
public:
    explicit MBoxData001(const std::string& filepath);
    ~MBoxData001() override;

    void RunTest() override;
private:

};

} // namespace Tests
