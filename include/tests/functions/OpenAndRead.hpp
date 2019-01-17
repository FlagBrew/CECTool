#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class OpenAndRead : public TestBase {
public:
    explicit OpenAndRead(const std::string& filepath);
    ~OpenAndRead() override;

    void RunTest() override;
private:

};

} // namespace Tests
