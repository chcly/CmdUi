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
#include "CmdUi/ColorCache.h"
#include "CmdUi/Math.h"

namespace Rt2::CmdUi
{
    ColorCacheItem::ColorCacheItem(const uint32_t color) :
        _color(color)
    {
    }

    ColorCacheItem::~ColorCacheItem() = default;

    const char* ColorCacheItem::foreground()
    {
        if (_fg.empty())
            _fg = Color(_color).foreground();
        return _fg.c_str();
    }

    const char* ColorCacheItem::background()
    {
        if (_bg.empty())
            _bg = Color(_color).background();
        return _bg.c_str();
    }
} // namespace Rt2::Ui
