#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class WriteMessageWithHMAC : public TestBase {
public:
    explicit WriteMessageWithHMAC(const std::string& filepath);
    ~WriteMessageWithHMAC() override;

    void RunTest() override;
private:

};

} // namespace Tests
