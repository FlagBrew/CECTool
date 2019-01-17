#pragma once

#include <3ds.h>
#include <string>

namespace Tests {

static const std::string rootTestsPath = "/3ds/CECTool/tests/";

void RunAllTests();

void RunAllFileTests();

void RunAllFunctionTests();

} // namespace Tests
