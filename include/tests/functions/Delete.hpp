#pragma once

#include "tests/testbase.hpp"

namespace Tests {

class Delete : public TestBase {
public:
    explicit Delete(const std::string& filepath);
    ~Delete() override;

    void RunTest() override;
private:

};

} // namespace Tests
