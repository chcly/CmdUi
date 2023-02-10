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
#ifndef WIN32
    #include "CmdUi/PlatformUnix.h"
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
    #include <cassert>
    #include <csignal>
    #include <cstdio>
    #include <cstdlib>

namespace Rt2::CmdUi
{
    constexpr int StdInput  = 0;
    constexpr int StdOutput = 1;

    class Terminal
    {
    private:
        static Terminal* _inst;
        static bool      _resize;
        static bool      _exitRequest;
        termios          _oldTerm;
        termios          _term;

    public:
        Terminal() :
            _oldTerm{},
            _term{}
        {
            _inst = this;
        }

        ~Terminal()
        {
            _inst = nullptr;
            tcsetattr(StdInput, TCSANOW, &_oldTerm);
        }

        void initialize()
        {
            tcgetattr(StdInput, &_oldTerm);

            _term = _oldTerm;

            _term.c_lflag &= ~ICANON;
            _term.c_lflag &= ~ECHO;

            // no-blocking: read returns immediately
            _term.c_cc[VMIN]  = 0;
            _term.c_cc[VTIME] = 0;
            tcsetattr(StdInput, TCSANOW, &_term);
        }

        void wake()
        {
            _term.c_cc[VMIN]  = 0;
            _term.c_cc[VTIME] = 0;

            tcsetattr(StdInput, TCSANOW, &_term);
            tcflush(StdInput, TCIOFLUSH);

            fputc(' ', stdin);
            fputc('\n', stdin);
            fflush(stdin);
        }

        void update(bool block)
        {
            int oldMin = _term.c_cc[VMIN];
            if (block)
            {
                if (oldMin == 0)
                {
                    // MIN > 0, TIME == 0 (blocking read)
                    _term.c_cc[VMIN]  = 1;
                    _term.c_cc[VTIME] = 0;
                    tcsetattr(StdInput, TCSANOW, &_term);
                }
            }
            else
            {
                if (oldMin != 0)
                {
                    _term.c_cc[VMIN]  = 0;
                    _term.c_cc[VTIME] = 0;
                    tcsetattr(StdInput, TCSANOW, &_term);
                }
            }
        }

        void reset()
        {
            _resize = false;
        }

        static void exitHandler(int)
        {
            _exitRequest = true;
            if (_inst)
                _inst->wake();
        }

        static void exitHandler()
        {
            exitHandler(0);
        }

        static void sizeHandler(int)
        {
            _resize = true;
            if (_inst)
                _inst->wake();
        }

        bool resized()
        {
            return _resize;
        }

        bool exitRequested()
        {
            return _exitRequest;
        }
    };

    Terminal* Terminal::_inst        = nullptr;
    bool      Terminal::_resize      = false;
    bool      Terminal::_exitRequest = false;

    PlatformUnix::PlatformUnix() :
        _term(nullptr),
        _size{0, 0}
    {
        saveState();
    }

    PlatformUnix::~PlatformUnix()
    {
        restoreState();
    }

    void PlatformUnix::saveState()
    {
        _term = new Terminal();
        _term->initialize();

        signal(SIGINT, Terminal::exitHandler);
        signal(SIGTERM, Terminal::exitHandler);
        signal(SIGQUIT, Terminal::exitHandler);
        signal(SIGKILL, Terminal::exitHandler);
        signal(SIGWINCH, Terminal::sizeHandler);
        atexit(Terminal::exitHandler);

        useBackBuffer(true);
    }

    void PlatformUnix::restoreState()
    {
        useBackBuffer(false);

        delete _term;
        _term = nullptr;
    }

    bool PlatformUnix::checkSize()
    {
        struct winsize ws;
        ioctl(StdOutput, TIOCGWINSZ, &ws);

        if (ws.ws_col != _size.x || ws.ws_row != _size.y)
        {
            _size.x = ws.ws_col;
            _size.y = ws.ws_row;
            return true;
        }
        return false;
    }

    void PlatformUnix::screenSizeHint(Vec2I& sz)
    {
        checkSize();
        sz = _size;
    }

    int PlatformUnix::scan(const char* cp, int len)
    {
        int  c  = 0;
        char ch = *cp++, nch;
        if (ch == '\x1b' && *cp == '\x5b')
        {
            ++cp;
            ch  = *cp++;
            nch = *cp;
            if (ch == '1' && nch == ';')
            {
                ++cp;
                ch  = *cp++;
                nch = *cp;
                if (ch == '5' && nch == 'A')
                    return PR_UP_ARROW;
                if (ch == '5' && nch == 'B')
                    return PR_DOWN_ARROW;
                if (ch == '5' && nch == 'C')
                    return PR_RIGHT_ARROW;
                if (ch == '5' && nch == 'D')
                    return PR_LEFT_ARROW;
            }
            else if (ch == 'A')
                return PR_UP_ARROW;
            else if (ch == 'B')
                return PR_DOWN_ARROW;
            else if (ch == 'C')
                return PR_RIGHT_ARROW;
            else if (ch == 'D')
                return PR_LEFT_ARROW;
            else if (ch == 'c')
                return PR_EXIT;
            else
                return PR_NO_INPUT;
        }
        else if (ch >= 'A' && ch <= 'Z')
        {
            ch -= 'A';
            return PR_A + ch;
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            ch -= 'a';
            return PR_A + ch;
        }
        else if (ch >= '0' && ch <= '9')
        {
            ch -= '0';
            return PR_0 + ch;
        }
        else if (ch == '\x1b')
            return PR_ESC;
        return ch;
    }

    int PlatformUnix::readStdin()
    {
        fd_set fd;
        FD_ZERO(&fd);
        FD_SET(StdInput, &fd);

        timeval tv{};
        tv.tv_sec = -1;
        select(1, &fd, nullptr, nullptr, &tv);
        if (FD_ISSET(StdInput, &fd))
        {
            char buf[128];
            int  br = read(StdInput, buf, 127);
            if (br > 0 && br < 128)
            {
                buf[br] = 0;
                return scan(buf, br);
            }
        }
        return PR_NO_INPUT;
    }

    int PlatformUnix::checkSignals()
    {
        if (_term->exitRequested())
            return PR_EXIT;

        if (_term->resized())
        {
            _term->reset();
            return PR_RESIZE;
        }
        return PR_NO_INPUT;
    }

    int PlatformUnix::poll(bool block)
    {
        int evt = checkSignals();
        if (evt != PR_NO_INPUT)
            return evt;

        _term->update(block);

        evt = readStdin();
        if (evt != PR_EXIT && evt != PR_RESIZE)
            return evt;

        evt = checkSignals();
        if (evt != PR_NO_INPUT)
            return evt;
        return PR_NO_INPUT;
    }

}  // namespace Rt2::CmdUi

#endif
