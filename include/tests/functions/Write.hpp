#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Write : public TestBase {
public:
    explicit Write(const std::string& filepath);
    ~Write() override;

    void RunTest() override;
private:

};

} // namespace Tests
