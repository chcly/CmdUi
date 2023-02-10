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
#include <unordered_map>
#include "CmdUi/ColorCache.h"
#include "CmdUi/Math.h"

namespace Rt2::CmdUi
{
    namespace Detail
    {
        constexpr const char* ShowCursor     = "\x1b[?25h";
        constexpr const char* HideCursor     = "\x1b[?25l";
        constexpr const char* UseBackBuffer  = "\x1b[?1049h";
        constexpr const char* UseFrontBuffer = "\x1b[?1049l";
        constexpr const char* Clear          = "\x1b[2J";
        constexpr const char* ZeroCursor     = "\x1b[0;0H";
        constexpr const char* ResetColor     = "\x1b[0m";
        constexpr const char* SetCursor      = "\x1b[%d;%dH";

        // line drawing
        constexpr const char* LineHz = "\x1b(0q";
        constexpr const char* LineVt = "\x1b(0x";

        constexpr const char* LineLt = "\x1b(0l";
        constexpr const char* LineRt = "\x1b(0k";
        constexpr const char* LineRb = "\x1b(0j";
        constexpr const char* LineLb = "\x1b(0m";

        constexpr const char* LineCr = "\x1b(0u";
        constexpr const char* LineCl = "\x1b(0t";
        constexpr const char* LineCt = "\x1b(0w";
        constexpr const char* LineCb = "\x1b(0v";
    }  // namespace Detail

    class Platform
    {
    public:
        typedef std::unordered_map<uint8_t, ColorCacheItem*> ColorCache;

    private:
        ColorCache _cache;

    public:
        Platform();
        virtual ~Platform();

        virtual void screenSizeHint(Vec2I& sz) = 0;

        virtual void buffersResized(size_t size);

        virtual int poll(bool block) = 0;

        virtual void put(char ch);

        virtual void color(uint8_t index, uint32_t color, bool background);

        virtual void put(const char* ch);

        virtual void setCursor(int16_t x, int16_t y);

        virtual void clear();

        virtual void resetCursor();

        virtual void resetColor();

        virtual void showCursor(bool show);

        virtual void useBackBuffer(bool use);

        virtual void flush();

        static void sleep(const uint32_t& ms);
    };
}  // namespace Rt2::CmdUi
