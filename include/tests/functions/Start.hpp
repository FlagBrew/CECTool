#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Start : public TestBase {
public:
    explicit Start(const std::string& filepath);
    ~Start() override;

    void RunTest() override;
private:

};

} // namespace Tests
