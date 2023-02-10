#include <cstdio>
#include "CmdUi/Context.h"
#include "CmdUi/PlatformFile.h"
#include "ThisDir.h"
#include "gtest/gtest.h"
#include "Utils/Console.h"

void CompareStreams(const Rt2::StringStream& a, Rt2::IStream& b)
{
    const Rt2::String sa = a.str();
    b.seekg(0, std::ios::end);
    const size_t          la = sa.size();
    const std::streamsize lb = b.tellg();

    b.seekg(0);

    EXPECT_EQ(la, lb);

    const char* pa = sa.data();

    char* pb = new char[lb + 1];
    b.read(pb, lb);

    const std::streamsize n = std::min<std::streamsize>((std::streamsize)la, lb);
    std::streamsize i = 0;
    while (i < n)
    {
        Rt2::Console::put(pa[i]);

        EXPECT_EQ(pa[i], pb[i]);
        ++i;
    }
    delete[] pb;
}

GTEST_TEST(CmdUi, Context_001)
{
    const auto plat = new Rt2::CmdUi::PlatformFile(Rt2::String(AbsTestFile("test_001.txt")));
    plat->setScreenSize(21, 5);
    const Rt2::CmdUi::Context ctx(plat);

    ctx.clear();
    ctx.rectangle(0, 0, 20, 4);
    ctx.string("Hello World", 2, 1);
    ctx.line(1, 2, 19, 0);
    ctx.string("a^2+b^2=c^2", 2, 3);
    ctx.flush();
    plat->close();

    Rt2::StringStream ss;
    ss << "+-------------------+\n";
    ss << "| Hello World       |\n";
    ss << "|-------------------|\n";
    ss << "| a^2+b^2=c^2       |\n";
    ss << "+-------------------+\n";

    auto ifs = Rt2::InputFileStream(AbsTestFile("test_001.txt"));
    CompareStreams(ss, ifs);
}
