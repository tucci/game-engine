#pragma once

#include "SDL.h"
#include <stdbool.h>
#include "Math/Vec.h"

typedef enum MouseButton {
	MouseButton_None,
	MouseButton_Left,
	MouseButton_Middle,
	MouseButton_Right,

} MouseButton;

typedef struct ButtonState {
	bool down;
	bool just_pressed;
	bool just_released;
} ButtonState;



typedef struct Mouse {
	Vec2i pos;
	Vec2i delta_pos;

	Vec2i global_pos;
	Vec2i global_delta_pos;

	Vec2i scroll;

	ButtonState mouse_button_left;
	ButtonState mouse_button_middle;
	ButtonState mouse_button_right;

} Mouse;


typedef struct Input {
	Mouse mouse;
	ButtonState keys[SDL_NUM_SCANCODES];
} Input;