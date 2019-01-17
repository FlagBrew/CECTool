#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetCecInfoEventHandle : public TestBase {
public:
    explicit GetCecInfoEventHandle(const std::string& filepath);
    ~GetCecInfoEventHandle() override;

    void RunTest() override;
private:

};

} // namespace Tests
