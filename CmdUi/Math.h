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
#include <vector>
#include "CmdUi/Common.h"

namespace Rt2::CmdUi
{
    class Vec2I
    {
    public:
        int x{}, y{};

    public:
        Vec2I()                 = default;
        Vec2I(const Vec2I& oth) = default;
        Vec2I(const int& nx, const int& ny) :
            x(nx), y(ny)
        {
        }

        Vec2I maxPoint(const Vec2I& rhs) const;
        Vec2I minPoint(const Vec2I& rhs) const;
    };

    using Path = std::vector<Vec2I>;

    class RectI
    {
    public:
        int x{}, y{}, w{}, h{};

    public:
        RectI()                 = default;
        RectI(const RectI& oth) = default;
        RectI(const int& nx,
              const int& ny,
              const int& nw,
              const int& nh) :
            x(nx), y(ny), w(nw), h(nh)
        {
        }

        int x1() const;

        int x2() const;

        int y1() const;

        int y2() const;

        int cx() const;

        int cy() const;

        Vec2I position() const;

        Vec2I size() const;
    };

    union ColorU
    {
        uint32_t i{};
        uint8_t  b[4];

        explicit ColorU(const uint32_t v) :
            b{0, 0, 0, 0}
        {
            i = v;
        }

        explicit ColorU(const uint8_t r,
                        const uint8_t g,
                        const uint8_t b) :
            b{r, g, b, 0}
        {
        }

        explicit operator uint32_t() const
        {
            return i;
        }
    };

    union ColorBuffer
    {
        uint16_t s{};
        uint8_t  b[2];
    };

    class Color
    {
    public:
        uint32_t Rgb{};

    private:
        mutable char _scratch[32]{};

        const char* extractSequence(const char* buf) const;

    public:
        Color() = default;

        explicit Color(const uint32_t& v) :
            Rgb(v)
        {
        }

        Color(const uint8_t r,
              const uint8_t g,
              const uint8_t b) :
            Rgb(ColorU(r, g, b))
        {
        }

        const char* foreground() const;

        const char* background() const;
    };

    inline int Clamp(const int& x, const int& a, const int& b)
    {
        return x < a ? a : (x > b ? b : x);
    }

    inline int RectI::x1() const
    {
        return x;
    }

    inline int RectI::x2() const
    {
        return x + w;
    }

    inline int RectI::y1() const
    {
        return y;
    }

    inline int RectI::y2() const
    {
        return y + h;
    }

    inline int RectI::cx() const
    {
        return x + (w >> 1);
    }

    inline int RectI::cy() const
    {
        return y + (h >> 1);
    }

    inline Vec2I RectI::position() const
    {
        return {x, y};
    }

    inline Vec2I RectI::size() const
    {
        return {w, h};
    }

}  // namespace Rt2::CmdUi
