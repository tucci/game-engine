#pragma once

#include <SDL.h>
#include "Math/Vec.h"
#include "types.h"

enum WindowFlag {
	// NOTE:(steven) These flags try to have the same values as the flags from SDL_WindowFlags
	WindowFlag_None = 0,
	WindowFlag_Fullscreen = 1 << 0,
	WindowFlag_OpenGL = 1 << 1,
	WindowFlag_Hidden = 1 << 3,
	WindowFlag_Borderless = 1 << 4,
	WindowFlag_Resizable = 1 << 5,
	WindowFlag_Minimized = 1 << 6,
	WindowFlag_Maximized = 1 << 7,
	WindowFlag_Has_Keyboard_Focus = 1 << 9,
	WindowFlag_Has_Mouse_Focus = 1 << 10,

};

struct Window {
	u32 window_id;

	const char* title;
	Vec2i pos;
	Vec2i size;
	u32 flags;

	SDL_Window* sdl_window;
	SDL_WindowFlags sdl_window_flags;

};



inline SDL_WindowFlags convert_to_sdl_flags(u32 flags) {
	return (SDL_WindowFlags) flags;
}

inline Vec2i get_window_size(Window* window) {
	return window->size;
}

inline bool window_has_focus(Window* window) {
	// NOTE:  the bit checking must be inside () because =! has higher precendance than the & operator
	return (window->flags & WindowFlag_Has_Mouse_Focus) != 0;
}