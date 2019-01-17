#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class WriteMessage : public TestBase {
public:
    explicit WriteMessage(const std::string& filepath);
    ~WriteMessage() override;

    void RunTest() override;
private:

};

} // namespace Tests
