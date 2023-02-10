/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#pragma once
#include <cstdint>

namespace Rt2::CmdUi
{
    class Platform;

    constexpr uint8_t AByte = 3;
    constexpr uint8_t RByte = 2;
    constexpr uint8_t GByte = 1;
    constexpr uint8_t BByte = 0;

    constexpr char RgbFormatFg[] = "\x1b[38;2;%d;%d;%dm";
    constexpr char RgbFormatBg[] = "\x1b[48;2;%d;%d;%dm";

    enum CharacterSet
    {
        CS_SEQ_START = -1,
        CS_NONE,
        CS_RECT_RB,
        CS_RECT_RT,
        CS_RECT_LT,
        CS_RECT_LB,
        CS_RECT_CS,
        CS_RECT_HZ,
        CS_RECT_CL,
        CS_RECT_CR,
        CS_RECT_CB,
        CS_RECT_CT,
        CS_RECT_VT,
        CS_SEQ_END,  //< CS_START

        // printable
        CS_START = ' ',
        CS_END   = '~' + 1,
    };

    enum ColorPalette
    {
        CP_TRANSPARENT = 0,
        CP_BLACK,
        CP_DARK_BLUE,
        CP_DARK_GREEN,
        CP_DARK_CYAN,
        CP_DARK_RED,
        CP_DARK_MAGENTA,
        CP_DARK_YELLOW,
        CP_GREY,
        CP_LIGHT_GREY,
        CP_BLUE,
        CP_GREEN,
        CP_CYAN,
        CP_RED,
        CP_MAGENTA,
        CP_YELLOW,
        CP_WHITE,
        CP_USER,
        // custom range [CP_USER .. 256]
        CP_MAX = 256,
    };

    enum Orientation
    {
        OR_HORIZONTAL,
        OR_VERTICAL,
    };

    enum PollResult
    {
        PR_EXIT = 0,
        PR_NO_INPUT,
        PR_RESIZE,
        PR_0 = '0',
        PR_1,
        PR_2,
        PR_3,
        PR_4,
        PR_5,
        PR_6,
        PR_7,
        PR_8,
        PR_9,

        PR_A = 'a',
        PR_B,
        PR_C,
        PR_D,
        PR_E,
        PR_F,
        PR_G,
        PR_H,
        PR_I,
        PR_J,
        PR_K,
        PR_L,
        PR_M,
        PR_N,
        PR_O,
        PR_P,
        PR_Q,
        PR_R,
        PR_S,
        PR_T,
        PR_U,
        PR_V,
        PR_W,
        PR_X,
        PR_Y,
        PR_Z,

        PR_UP_ARROW,
        PR_DOWN_ARROW,
        PR_RIGHT_ARROW,
        PR_LEFT_ARROW,

        PR_CTRL,
        PR_SHIFT,
        PR_ALT,
        PR_ESC,
    };
}  // namespace Rt2::CmdUi
