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
#ifndef WIN32
    #include <termios.h>
    #include "CmdUi/Math.h"
    #include "CmdUi/Platform.h"

struct termios;

namespace Rt2::CmdUi
{
    class PlatformUnix final : public Platform
    {
    private:
        class Terminal* _term;
        Vec2I           _size;
        String          _message;

        void saveState();

        void restoreState();

        bool checkSize();

        int readStdin();

        int scan(const char* cp, int len);

        int checkSignals();

    public:
        PlatformUnix();
        ~PlatformUnix() override;

        void screenSizeHint(Vec2I& sz) override;

        int poll(bool block) override;
    };

}  // namespace Rt2::CmdUi
#endif