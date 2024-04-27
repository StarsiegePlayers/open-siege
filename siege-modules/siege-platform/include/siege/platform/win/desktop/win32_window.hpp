#ifndef WIN32_WINDOW_HPP
#define WIN32_WINDOW_HPP

#include <siege/platform/win/gaming/win32_user32.hpp>

namespace win32
{
	struct window
	{
    	hwnd_t self;

        window() : self(nullptr)
        {
        }

        explicit window(hwnd_t self) : self(self)
        {
        }

        operator hwnd_t()
        {
            return self;
        }

        operator bool()
        {
            return self != nullptr;
        }
	};
}


#endif // !WIN32_WINDOW_HPP
