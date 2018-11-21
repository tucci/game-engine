#pragma once

#include <SDL.h>
#include "types.h"

typedef enum WindowFlag {
	// NOTE:(steven) These flags try to have the same values as the flags from SDL_WindowFlags
	WindowFlag_None = 0,
	WindowFlag_Fullscreen = 1 << 0,
	WindowFlag_OpenGL = 1 << 1,
	WindowFlag_Hidden = 1 << 3,
	WindowFlag_Resizable = 1 << 5,
	WindowFlag_Minimized = 1 << 6,
	WindowFlag_Maximized = 1 << 7,
	WindowFlag_Has_Keyboard_Focus = 1 << 9,
	WindowFlag_Has_Mouse_Focus = 1 << 10,

} WindowFlag;

typedef struct Window {
	u32 window_id;

	const char* title;
	Vec2i pos;
	Vec2i size;
	u32 flags;

	SDL_Window* sdl_window;
	SDL_WindowFlags sdl_window_flags;

} Window;
