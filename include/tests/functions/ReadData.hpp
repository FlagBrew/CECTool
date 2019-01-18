#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class ReadData : public TestBase {
public:
    explicit ReadData(const std::string& filepath);
    ~ReadData() override;

    void RunTest() override;
private:

};

} // namespace Tests
