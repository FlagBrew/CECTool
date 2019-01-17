#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class ReadMessageWithHMAC : public TestBase {
public:
    explicit ReadMessageWithHMAC(const std::string& filepath);
    ~ReadMessageWithHMAC() override;

    void RunTest() override;
private:

};

} // namespace Tests
