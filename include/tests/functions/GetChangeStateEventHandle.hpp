#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetChangeStateEventHandle : public TestBase {
public:
    explicit GetChangeStateEventHandle(const std::string& filepath);
    ~GetChangeStateEventHandle() override;

    void RunTest() override;
private:

};

} // namespace Tests
