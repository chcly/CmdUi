#include <cstdio>
#include "CmdUi/Context.h"
#include "CmdUi/PlatformFile.h"
#include "gtest/gtest.h"
#include "TestDirectory.h"

GTEST_TEST(Test1, AlwaysTrue)
{
    Rt2::CmdUi::Context ctx(new Rt2::CmdUi::PlatformFile(GetTestFilePath("test_001.txt")));
    ctx.clear();
    ctx.rectangle(1, 1, 20, 5);
    ctx.flush();
}
