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
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <cstdio>
#include "CmdUi/Platform.h"

namespace Rt2::CmdUi
{
    Platform::Platform() = default;

    Platform::~Platform()
    {
        for (auto& [idx, item] : _cache)
            delete item;

        std::setvbuf(stdout, nullptr, _IOLBF, 0x400);
    }

    void Platform::color(uint8_t index, const uint32_t color, const bool background)
    {
        ColorCacheItem* item;

        const ColorCache::iterator it = _cache.find(index);
        if (it == _cache.end())
        {
            item = new ColorCacheItem(color);
            _cache.insert(std::make_pair(index, item));
        }
        else
            item = it->second;

        if (background)
            put(item->background());
        else
            put(item->foreground());
    }

    void Platform::put(const char ch)
    {
        if (ch >= CS_START && ch < CS_END)
            putchar(ch);
        else if (ch > CS_SEQ_START && ch < CS_SEQ_END)
        {
            switch (ch)
            {
            case CS_NONE:
                put(' ');
                break;
            case CS_RECT_HZ:
                put(Detail::LineHz);
                break;
            case CS_RECT_VT:
                put(Detail::LineVt);
                break;
            case CS_RECT_LT:
                put(Detail::LineLt);
                break;
            case CS_RECT_RT:
                put(Detail::LineRt);
                break;
            case CS_RECT_RB:
                put(Detail::LineRb);
                break;
            case CS_RECT_LB:
                put(Detail::LineLb);
                break;
            case CS_RECT_CL:
                put(Detail::LineCl);
                break;
            case CS_RECT_CR:
                put(Detail::LineCr);
                break;
            case CS_RECT_CT:
                put(Detail::LineCt);
                break;
            case CS_RECT_CB:
                put(Detail::LineCb);
                break;
            default:
                putchar('?');
                break;
            }
        }
        else
            putchar('?');
    }

    void Platform::put(const char* ch)
    {
        printf("%s", ch);
    }

    void Platform::setCursor(const int16_t x, const int16_t y)
    {
        printf(Detail::SetCursor, x, y);
    }

    void Platform::showCursor(const bool show)
    {
        if (show)
            put(Detail::ShowCursor);
        else
            put(Detail::HideCursor);
    }

    void Platform::resetCursor()
    {
        put(Detail::ZeroCursor);
    }

    void Platform::resetColor()
    {
        put(Detail::ResetColor);
    }

    void Platform::clear()
    {
        put(Detail::Clear);
    }

    void Platform::useBackBuffer(const bool use)
    {
        if (use)
        {
            put(Detail::Clear);
            put(Detail::HideCursor);
            put(Detail::ZeroCursor);
            put(Detail::UseBackBuffer);
            flush();
        }
        else
        {
            put(Detail::Clear);
            put(Detail::ShowCursor);
            put(Detail::ZeroCursor);
            put(Detail::UseFrontBuffer);
            flush();
        }
    }

    void Platform::flush()
    {
        putchar('\0');
        fflush(stdout);
    }

    void Platform::sleep(const uint32_t& ms)
    {
#ifdef WIN32
        Sleep(ms);
#else
        usleep(ms * 1000);
#endif
    }
}  // namespace Rt2::Ui
