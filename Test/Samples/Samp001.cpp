#include "CmdUi/Context.h"
using namespace Rt2;

int main(int argc, char** argv)
{
    CmdUi::Context ctx;
    ctx.createColor(CmdUi::Color(0x764D9FFF));

    int rc = 0;
    while (rc != CmdUi::PR_ESC)
    {
        ctx.background(CmdUi::CP_USER);
        ctx.clear();
        ctx.foreground(CmdUi::CP_LIGHT_GREY);
        ctx.background(CmdUi::CP_BLACK);
        ctx.string("Hello World", 2, 2);
        ctx.flush();

        rc = ctx.poll(true);
    }
    return 0;
}