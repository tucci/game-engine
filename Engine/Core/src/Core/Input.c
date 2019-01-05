#pragma once

#include "Core/Input.h"


bool is_key_down(Input* input, Keycode keycode) {
	SDL_Keycode sdl_keycode = keycode_to_sdl_keycode(keycode);
	SDL_Scancode scancode = SDL_GetScancodeFromKey(sdl_keycode);
	return input->keys[scancode].down;
}

bool is_key_released(Input* input, Keycode keycode) {
	SDL_Keycode sdl_keycode = keycode_to_sdl_keycode(keycode);
	SDL_Scancode scancode = SDL_GetScancodeFromKey(sdl_keycode);
	return input->keys[scancode].just_released;
}

bool is_key_pressed(Input* input, Keycode keycode) {
	SDL_Keycode sdl_keycode = keycode_to_sdl_keycode(keycode);
	SDL_Scancode scancode = SDL_GetScancodeFromKey(sdl_keycode);
	return input->keys[scancode].just_pressed;
}


bool is_mouse_down(Input* input, MouseButton which) {
	
	bool down = false;
	switch (which) {
		case MouseButton_None: {
			// do nothing
			break;
		}
		case MouseButton_Left: {
			down = input->mouse.mouse_button_left.down;
			break;
		}
		case MouseButton_Middle: {
			down = input->mouse.mouse_button_middle.down;
			break;
		}
		case MouseButton_Right: {
			down = input->mouse.mouse_button_right.down;
			break;
		}
		default:
		{
			break;
		}

	}
	
	return down;
}

bool is_mouse_released(Input* input, MouseButton which) {
	bool released = false;
	switch (which) {
		case MouseButton_None: {
			// do nothing
			break;
		}
		case MouseButton_Left: {
			released = input->mouse.mouse_button_left.just_released;
			break;
		}
		case MouseButton_Middle: {
			released = input->mouse.mouse_button_middle.just_released;
			break;
		}
		case MouseButton_Right: {
			released = input->mouse.mouse_button_right.just_released;
			break;
		}
		default:
		{
			break;
		}

	}

	return released;
}

bool is_mouse_pressed(Input* input, MouseButton which) {
	bool pressed = false;
	switch (which) {
		case MouseButton_None: {
			// do nothing
			break;
		}
		case MouseButton_Left: {
			pressed = input->mouse.mouse_button_left.just_pressed;
			break;
		}
		case MouseButton_Middle: {
			pressed = input->mouse.mouse_button_middle.just_pressed;
			break;
		}
		case MouseButton_Right: {
			pressed = input->mouse.mouse_button_right.just_pressed;
			break;
		}
		default:
		{
			break;
		}

	}

	return pressed;
}

Vec2i get_scroll_delta(Input* input) {
	return input->mouse.scroll;
}


Vec2i get_mouse_pos(Input* input) {
	return input->mouse.pos;
}
Vec2i get_mouse_delta(Input* input) {
	return input->mouse.delta_pos;
}


Vec2i get_global_mouse_pos(Input* input) {
	return input->mouse.global_pos;
}

Vec2i get_global_mouse_delta(Input* input) {
	return input->mouse.global_delta_pos;
}
