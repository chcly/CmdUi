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
#include "CmdUi/PlatformFile.h"
#include "CmdUi/Math.h"
#include "Utils/Char.h"

namespace Rt2::CmdUi
{
    PlatformFile::PlatformFile(const String& name) :
        _ofs(new OutputFileStream(name, std::ios::binary))
    {
        if (!_ofs->is_open())
            close();
    }

    PlatformFile::~PlatformFile()
    {
        close();
    }

    void PlatformFile::screenSizeHint(Vec2I& sz)
    {
        sz = _size;
    }

    void PlatformFile::put(const char ch)
    {
        if (_ofs)
        {
            _ofs->put(ch);
            if (_writes % _size.x == _size.x - 1)
                _ofs->put('\n');
            ++_writes;
        }
    }

    void PlatformFile::put(const char* ch)
    {
        const size_t len = Char::length(ch);
        for (size_t i = 0; i < len; ++i)
            PlatformFile::put(ch);
    }

    void PlatformFile::flush()
    {
        if (_ofs)
        {
            _writes = 0;
            _ofs->seekp(0, std::ios::beg);
        }
    }

    int PlatformFile::poll(bool block)
    {
        // stub, undefined usage for files
        return -1;
    }

    void PlatformFile::showCursor(bool show)
    {
        // stub, undefined usage for files
    }

    void PlatformFile::useBackBuffer(bool use)
    {
        // stub, undefined usage for files
    }

    void PlatformFile::setCursor(int16_t x, int16_t y)
    {
        // stub, undefined usage for files
    }

    void PlatformFile::resetCursor()
    {
        // stub, undefined usage for files
    }

    void PlatformFile::close()
    {
        delete _ofs;
        _ofs = nullptr;
    }

    void PlatformFile::setScreenSize(int w, int h)
    {
        _size = {w, h};
    }

}  // namespace Rt2::CmdUi
