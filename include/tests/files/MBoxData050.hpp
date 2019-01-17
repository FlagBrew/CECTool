#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class MBoxData050 : public TestBase {
public:
    explicit MBoxData050(const std::string& filepath);
    ~MBoxData050() override;

    void RunTest() override;
private:

};

} // namespace Tests
