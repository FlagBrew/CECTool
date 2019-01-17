#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class ReadMessage : public TestBase {
public:
    explicit ReadMessage(const std::string& filepath);
    ~ReadMessage() override;

    void RunTest() override;
private:

};

} // namespace Tests
