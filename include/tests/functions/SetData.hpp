#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class SetData : public TestBase {
public:
    explicit SetData(const std::string& filepath);
    ~SetData() override;

    void RunTest() override;
private:

};

} // namespace Tests
