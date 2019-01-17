#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Open : public TestBase {
public:
    explicit Open(const std::string& filepath);
    ~Open() override;

    void RunTest() override;
private:

};

} // namespace Tests
