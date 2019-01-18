#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class OpenAndWrite : public TestBase {
public:
    explicit OpenAndWrite(const std::string& filepath);
    ~OpenAndWrite() override;

    void RunTest() override;
private:

};

} // namespace Tests
