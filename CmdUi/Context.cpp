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
#include <csignal>
#include <cstring>
#include <iostream>
#ifdef WIN32
    #include "CmdUi/PlatformWin32.h"
#else
    #include <unistd.h>
    #include "CmdUi/PlatformUnix.h"
#endif
#include <iomanip>
#include "CmdUi/Common.h"
#include "CmdUi/Context.h"
#include "CmdUi/Math.h"

namespace Rt2::CmdUi
{
    Context::Context() :
#ifdef WIN32
        _platform(new PlatformWin32())
#else
        _platform(PlatformUnix())
#endif

    {
        initializePalette();
        initialize();
        reset();
    }

    Context::Context(Platform* other) :
        _platform(other)  // takes ownership
    {
        initializePalette();
        initialize();
        reset();
    }

    Context::~Context()
    {
        delete[] _frameBuffer;
        _frameBuffer = nullptr;

        delete[] _colorMap;
        _colorMap = nullptr;

        delete[] _colorBuffer;
        _colorBuffer = nullptr;

        delete _platform;
    }

    void Context::initializePalette()
    {
        _cbSize = CP_USER;
        _cbCap  = _cbSize + 16;

        _colorMap = new uint32_t[_cbCap];
        memset(_colorMap, 0, _cbCap);

        _colorMap[CP_TRANSPARENT]  = 0x000000;
        _colorMap[CP_BLACK]        = 0x000000;
        _colorMap[CP_GREY]         = 0x808080;
        _colorMap[CP_LIGHT_GREY]   = 0xC0C0C0;
        _colorMap[CP_WHITE]        = 0xFFFFFF;
        _colorMap[CP_DARK_RED]     = 0x800000;
        _colorMap[CP_DARK_GREEN]   = 0x008000;
        _colorMap[CP_DARK_BLUE]    = 0x000080;
        _colorMap[CP_DARK_CYAN]    = 0x008080;
        _colorMap[CP_DARK_MAGENTA] = 0x800080;
        _colorMap[CP_DARK_YELLOW]  = 0x808000;
        _colorMap[CP_RED]          = 0xFF0000;
        _colorMap[CP_GREEN]        = 0x00FF00;
        _colorMap[CP_BLUE]         = 0x0000FF;
        _colorMap[CP_CYAN]         = 0x00FFFF;
        _colorMap[CP_MAGENTA]      = 0xFF00FF;
        _colorMap[CP_YELLOW]       = 0xFFFF00;
    }

    void Context::color(const uint8_t& fg, const uint8_t& bg)
    {
        foreground(fg);
        background(bg);
    }

    uint8_t Context::createColor(const Color& col)
    {
        if (_cbSize + 1 < 256)
        {
            if (_cbSize + 1 > _cbCap)
            {
                _cbCap *= 2;
                uint32_t* data = new uint32_t[_cbCap];

                memset(data, 0, _cbCap);
                memcpy(data, _colorMap, _cbSize * sizeof(uint32_t));

                delete[] _colorMap;
                _colorMap = data;
            }
            _colorMap[_cbSize++] = col.Rgb;
        }
        return (uint8_t)((int32_t)_cbSize - 1);
    }

    void Context::initialize()
    {
        Point sz;
        _platform->screenSizeHint(sz);

        resizeBuffers(sz);
    }

    void Context::resizeBuffers(const Point& newSize)
    {
        if (_frameBuffer != nullptr)
        {
            delete[] _frameBuffer;
            delete[] _colorBuffer;
            _frameBuffer = nullptr;
            _colorBuffer = nullptr;
            _capacity    = 0;
        }

        // Extract the total number of rows and columns
        // that fit in the current screen space.

        _size     = {newSize.x, newSize.y};
        _pitch    = _size.x;
        _capacity = (size_t)newSize.x * (size_t)_size.y;

        if (_capacity == 0)
            return;

        _frameBuffer = new char[_capacity + 2];
        _colorBuffer = new ColorBuffer[_capacity + 2];

        clear();
        std::setvbuf(stdout, nullptr, _IOFBF, _capacity);
    }

    void Context::insertCharacter(const char ch, int x, int y) const
    {
        if (x < _size.x && y < _size.y)
        {
            const size_t loc = (size_t)y * (size_t)_pitch + (size_t)x;
            if (loc < _capacity)
            {
                _frameBuffer[loc]      = ch;
                _colorBuffer[loc].b[0] = (uint8_t)_color.x;
                _colorBuffer[loc].b[1] = (uint8_t)_color.y;
            }
        }
    }

    void Context::character(const char ch,
                            const int  x,
                            const int  y) const
    {
        insertCharacter(ch, x, y);
    }

    void Context::character(char ch, const Point& pt) const
    {
        character(ch, pt.x, pt.y);
    }

    void Context::string(const String& str,
                         int           x,
                         int           y) const
    {
        for (char ch : str)
        {
            if (ch == '\t')
                x += 4;
            else if (ch == '\n' || ch == '\r')
                y++;
            else
                insertCharacter(ch, x++, y);
        }
    }

    void Context::string(const String& str, const Point& pt) const
    {
        string(str, pt.x, pt.y);
    }

    void Context::integer(const uint32_t& val, int x, int y, int width) const
    {
        OutputStringStream ss;
        if (width != -1)
            ss << std::setw(width) << std::right << val;
        else
            ss << val;
        string(ss.str(), x, y);
    }

    void Context::line(int x,
                       int y,
                       int length,
                       int orientation) const
    {
        if (orientation == OR_HORIZONTAL)
        {
            const int l = Clamp(length, 0, _size.x);

            for (int i = 0; i < l; ++i)
                insertCharacter(CS_RECT_HZ, x++, y);
        }
        else if (orientation == OR_VERTICAL)
        {
            const int l = Clamp(length, 0, _size.y);
            for (int i = 0; i < l; ++i)
                insertCharacter(CS_RECT_VT, x, y++);
        }
    }

    void Context::line(const Point& pt, int length, int orientation) const
    {
        line(pt.x, pt.y, length, orientation);
    }

    void Context::rectangle(const int x,
                            const int y,
                            const int w,
                            const int h) const
    {
        Rectangle r{x, y, w - 1, h - 1};

        line(r.left(), r.top(), r.w, OR_HORIZONTAL);
        line(r.left(), r.top(), r.h, OR_VERTICAL);
        line(r.left(), r.bottom(), r.w, OR_HORIZONTAL);
        line(r.right(), r.top(), r.h, OR_VERTICAL);

        character(CS_RECT_LT, r.left(), r.top());
        character(CS_RECT_RT, r.right(), r.top());
        character(CS_RECT_RB, r.right(), r.bottom());
        character(CS_RECT_LB, r.left(), r.bottom());
    }

    void Context::rectangle(const Rectangle& rect) const
    {
        rectangle(rect.x, rect.y, rect.w, rect.h);
    }

    void Context::moveTo(const int& x, const int& y)
    {
        clearPath();
        _workingPath.push_back({x, y});
    }

    void Context::lineTo(const int& x, const int& y, const int& ori)
    {
        if (_workingPath.empty())
            _workingPath.push_back({x, y});
        else
        {
            Point a = _workingPath.back();
            Point b = {x, y};

            const Point c = a.maxPoint(b);

            b = a.minPoint(b);
            a = c;
            if (ori == OR_HORIZONTAL)
                line(b.x, b.y, a.x - b.x, ori);
            else
                line(b.x, b.y, std::abs(b.y - a.y), ori);

            _workingPath.push_back({x, y});
        }
    }

    void Context::addSegment(char ch)
    {
        if (!_workingPath.empty())
        {
            Point& pt = _workingPath.back();
            character(ch, pt.x, pt.y);
        }
    }

    void Context::clearPath()
    {
        _workingPath.clear();
    }

    void Context::reset()
    {
        _useExtended = false;
        _color       = {CP_TRANSPARENT, CP_TRANSPARENT};
    }

    void Context::sleep(const uint32_t& ms)
    {
        Platform::sleep(ms);
    }

    int Context::poll(const bool block)
    {
        const int result = _platform->poll(block);

        if (result == PR_RESIZE)
        {
            Point sz;
            _platform->screenSizeHint(sz);
            resizeBuffers(sz);
        }
        return result;
    }

    void Context::putExtended(const char ch) const
    {
        if (_useExtended)
        {
            switch (ch)
            {
            case CS_NONE:
                _platform->put(' ');
                break;
            case CS_RECT_CS:
            case CS_RECT_CL:
            case CS_RECT_CR:
            case CS_RECT_CB:
            case CS_RECT_CT:
            case CS_RECT_HZ:
            case CS_RECT_VT:
            case CS_RECT_LT:
            case CS_RECT_RT:
            case CS_RECT_RB:
            case CS_RECT_LB:
                _platform->put(ch);
                _platform->put("\x1b(B");
                break;
            default:
                _platform->put('?');
                break;
            }
        }
        else
        {
            switch (ch)
            {
            case CS_NONE:
                _platform->put('\0');
                break;
            case CS_RECT_CS:
            case CS_RECT_CL:
            case CS_RECT_CR:
            case CS_RECT_CB:
            case CS_RECT_CT:
                _platform->put(' ');
                break;
            case CS_RECT_HZ:
                _platform->put('-');
                break;
            case CS_RECT_VT:
                _platform->put('|');
                break;
            case CS_RECT_LT:
            case CS_RECT_RT:
            case CS_RECT_RB:
            case CS_RECT_LB:
                _platform->put('+');
                break;
            default:
                _platform->put('?');
                break;
            }
        }
    }

    void Context::clear(const char& ch) const
    {
        if (_capacity > 0)
        {
            memset(_frameBuffer, ch, _capacity);
            _frameBuffer[_capacity + 1] = 0;

            if (_color.x == CP_TRANSPARENT && _color.y == CP_TRANSPARENT)
                memset(_colorBuffer, 0, _capacity * sizeof(ColorBuffer));
            else
            {
                size_t i = 0;
                while (i < _capacity)
                {
                    _colorBuffer[i].b[0] = (uint8_t)_color.x;
                    _colorBuffer[i].b[1] = (uint8_t)_color.y;
                    i++;
                }
            }
        }
    }

    void Context::flush() const
    {
        if (!_frameBuffer || _capacity == 0)
            return;

        _platform->showCursor(false);
        _platform->resetCursor();

        const size_t w = (size_t)_size.x;
        const size_t h = (size_t)_size.y;

        for (size_t y = 0; y < h; ++y)
        {
            const size_t stride = y * _pitch;

            for (size_t x = 0; x < w; ++x)
            {
                const size_t loc = stride + x;

                const bool hasColor = _colorBuffer[loc].s != 0;
                if (hasColor)
                {
                    uint8_t col = _colorBuffer[loc].b[0];
                    if (col != CP_TRANSPARENT && col < _cbSize)
                        _platform->color(col, _colorMap[col], false);

                    col = _colorBuffer[loc].b[1];
                    if (col != CP_TRANSPARENT && col < _cbSize)
                        _platform->color(col, _colorMap[col], true);
                }

                const char ch = _frameBuffer[loc];
                if (ch > CS_SEQ_START && ch < CS_START)
                    putExtended(ch);
                else if (ch > CS_START && ch < CS_END)
                    _platform->put(ch);
                else
                    _platform->put(' ');

                if (hasColor)
                    _platform->resetColor();
            }
        }
        _platform->flush();
    }
}  // namespace Rt2::CmdUi
