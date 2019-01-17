#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetCecdState : public TestBase {
public:
    explicit GetCecdState(const std::string& filepath);
    ~GetCecdState() override;

    void RunTest() override;
private:

};

} // namespace Tests
