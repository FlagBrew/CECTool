#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Read : public TestBase {
public:
    explicit Read(const std::string& filepath);
    ~Read() override;

    void RunTest() override;
private:

};

} // namespace Tests
