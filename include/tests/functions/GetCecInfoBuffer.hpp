#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class GetCecInfoBuffer : public TestBase {
public:
    explicit GetCecInfoBuffer(const std::string& filepath);
    ~GetCecInfoBuffer() override;

    void RunTest() override;
private:

};

} // namespace Tests
