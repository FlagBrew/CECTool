#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class MBoxData010 : public TestBase {
public:
    explicit MBoxData010(const std::string& filepath);
    ~MBoxData010() override;

    void RunTest() override;
private:

};

} // namespace Tests
