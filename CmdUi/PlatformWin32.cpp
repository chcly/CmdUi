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
#ifdef WIN32
#include "CmdUi/PlatformWin32.h"
#include <iostream>
#include "CmdUi/Common.h"

namespace Rt2::CmdUi
{
    static bool Hook = false;

    int ProcessKey(const KEY_EVENT_RECORD& keyEvent);

    PlatformWin32::PlatformWin32() :
        _stdout(GetStdHandle(STD_OUTPUT_HANDLE)),
        _stdin(GetStdHandle(STD_INPUT_HANDLE)),
        _outputMode(0),
        _inputMode(0)
    {
        saveState();
    }

    PlatformWin32::~PlatformWin32()
    {
        restoreState();
    }

    void PlatformWin32::saveState()
    {
        GetConsoleMode(_stdout, &_outputMode);
        DWORD current = _outputMode;
        current |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        current |= DISABLE_NEWLINE_AUTO_RETURN;
        current |= ENABLE_WRAP_AT_EOL_OUTPUT;
        SetConsoleMode(_stdout, current);

        GetConsoleMode(_stdin, &_inputMode);
        current = _inputMode;
        current |= ENABLE_WINDOW_INPUT;
        current &= ~ENABLE_LINE_INPUT;
        current &= ~ENABLE_ECHO_INPUT;
        SetConsoleMode(_stdin, current);

        SetConsoleCtrlHandler(
            [](const DWORD evt)
            {
                switch (evt)
                {
                case CTRL_C_EVENT:
                case CTRL_BREAK_EVENT:
                case CTRL_CLOSE_EVENT:
                    Hook = true;
                    [[fallthrough]];
                default:
                    break;
                }
                return Hook ? TRUE : FALSE;
            },
            TRUE);
        clear();
        useBackBuffer(true);
        flush();
    }

    void PlatformWin32::restoreState()
    {
        clear();
        useBackBuffer(false);
        SetConsoleMode(_stdout, _outputMode);
        SetConsoleMode(_stdin, _inputMode);
    }

    void PlatformWin32::screenSizeHint(Vec2I& sz)
    {
        sz = {0, 0};

        CONSOLE_SCREEN_BUFFER_INFO info{};

        if (GetConsoleScreenBufferInfo(_stdout, &info) != FALSE)
            sz = {info.dwSize.X, info.dwSize.Y};
    }

    int PlatformWin32::poll(const bool block)
    {
        if (block)
            WaitForSingleObject(_stdin, INFINITE);

        DWORD count;
        GetNumberOfConsoleInputEvents(_stdin, &count);

        if (count > 0)
        {
            INPUT_RECORD r[8];
            ReadConsoleInput(_stdin, r, 8, &count);
            bool resized = false;

            for (DWORD i = 0; i < count; ++i)
            {
                INPUT_RECORD& cur = r[i];
                if (cur.EventType == WINDOW_BUFFER_SIZE_EVENT)
                    resized = true;
                else if (!resized && !Hook)
                {
                    if (cur.EventType == KEY_EVENT)
                    {
                        if (const int evt = ProcessKey(cur.Event.KeyEvent); 
                            evt != PR_NO_INPUT)
                            return evt;
                    }
                }
            }

            if (resized && !Hook)
                return PR_RESIZE;
        }

        if (Hook)
            return PR_EXIT;

        return PR_NO_INPUT;
    }

    int ProcessKey(const KEY_EVENT_RECORD& keyEvent)
    {
        if (keyEvent.bKeyDown)
        {
            const int ch = keyEvent.wVirtualKeyCode;
            if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'z')
                return ch;

            if (ch >= 'A' && ch <= 'Z')
                return PR_A + (ch - 'A');

            switch (ch)
            {
            case VK_UP:
                return PR_UP_ARROW;
            case VK_DOWN:
                return PR_DOWN_ARROW;
            case VK_RIGHT:
                return PR_RIGHT_ARROW;
            case VK_LEFT:
                return PR_LEFT_ARROW;
            case VK_ESCAPE:
                return PR_ESC;
            default:
                break;
            }
        }

        return PR_NO_INPUT;
    }
}  // namespace Rt2::CmdUi
#endif
