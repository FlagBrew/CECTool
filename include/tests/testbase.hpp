#pragma once

#include <fstream>

namespace Tests {

class TestBase {
public:
    explicit TestBase(const std::string& filepath);
    virtual ~TestBase();

    virtual void RunTest();

    std::fstream log;
    std::string logpath;
private:

};

} // namespace Tests
