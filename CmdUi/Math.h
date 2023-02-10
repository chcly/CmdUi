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
    class Point
    {
    public:
        int x{}, y{};

    public:
        Point()                 = default;
        Point(const Point& oth) = default;

        Point(const int& nx, const int& ny) :
            x(nx),
            y(ny)
        {
        }

        Point maxPoint(const Point& rhs) const
        {
            return {
                std::max<int>(x, rhs.x),
                std::max<int>(y, rhs.y),
            };
        }

        Point minPoint(const Point& rhs) const
        {
            return {
                std::min<int>(x, rhs.x),
                std::min<int>(y, rhs.y),
            };
        }
    };

    using Path = std::vector<Point>;

    class Rectangle
    {
    public:
        int x{}, y{}, w{}, h{};

    public:
        Rectangle()                     = default;
        Rectangle(const Rectangle& oth) = default;

        Rectangle(const int& nx,
                  const int& ny,
                  const int& nw,
                  const int& nh) :
            x(nx),
            y(ny),
            w(nw),
            h(nh)
        {
        }

        int left() const
        {
            return x;
        }

        int right() const
        {
            return x + w;
        }

        int midX() const
        {
            return x + (w >> 1);
        }

        int top() const
        {
            return y;
        }

        int bottom() const
        {
            return y + h;
        }

        int midY() const
        {
            return y + (h >> 1);
        }

        Point position() const
        {
            return {x, y};
        }

        Point size() const
        {
            return {w, h};
        }
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

        explicit operator int() const
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
            Rgb((int)ColorU(r, g, b))
        {
        }

        const char* foreground() const
        {
            return extractSequence(RgbFormatFg);
        }

        const char* background() const
        {
            return extractSequence(RgbFormatBg);
        }
    };

    inline int Clamp(const int& in,
                     const int& start,
                     const int& end)
    {
        return in < start
                   ? start
                   : in > end
                   ? end
                   : in;
    }
} // namespace Rt2::Ui
