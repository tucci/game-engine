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

bool is_keymod_down(Input* input, Keymod keymod) {
	switch (keymod)
	{
	case KEYMOD_NONE:
		return false;
		break;
	case KEYMOD_LSHIFT:
		return is_key_down(input, KEYCODE_LSHIFT);
		break;
	case KEYMOD_RSHIFT:
		return is_key_down(input, KEYCODE_RSHIFT);
		break;
	case KEYMOD_LCTRL:
		return is_key_down(input, KEYCODE_LCTRL);
		break;
	case KEYMOD_RCTRL:
		return is_key_down(input, KEYCODE_RCTRL);
		break;
	case KEYMOD_LALT:
		return is_key_down(input, KEYCODE_LALT);
		break;
	case KEYMOD_RALT:
		return is_key_down(input, KEYCODE_RALT);
		break;
	case KEYMOD_LGUI:
		return is_key_down(input, KEYCODE_LGUI);
		break;
	case KEYMOD_RGUI:
		return is_key_down(input, KEYCODE_RGUI);
		break;
	case KEYMOD_NUM:
		return is_key_down(input, KEYCODE_NUMLOCKCLEAR);
		break;
	case KEYMOD_CAPS:
		return is_key_down(input, KEYCODE_CAPSLOCK);
		break;
	case KEYMOD_MODE:
		return is_key_down(input, KEYCODE_MODE);
		break;
	case KEYMOD_CTRL:
		return is_key_down(input, KEYCODE_LCTRL) || is_key_down(input, KEYCODE_RCTRL);
		break;
	case KEYMOD_SHIFT:
		return is_key_down(input, KEYCODE_LSHIFT) || is_key_down(input, KEYCODE_RSHIFT);
		break;
	case KEYMOD_ALT:
		return is_key_down(input, KEYCODE_LALT) || is_key_down(input, KEYCODE_RALT);
		break;
	case KEYMOD_GUI:
		return is_key_down(input, KEYCODE_LGUI) || is_key_down(input, KEYCODE_RGUI);
		break;
	default:
		break;
	}

}


bool is_mouse_down(Input* input, MouseButton which) {
	
	bool down = false;
	switch (which) {
		case MouseButton::None: {
			// do nothing
			break;
		}
		case MouseButton::Left: {
			down = input->mouse.mouse_button_left.down;
			break;
		}
		case MouseButton::Middle: {
			down = input->mouse.mouse_button_middle.down;
			break;
		}
		case MouseButton::Right: {
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
		case MouseButton::None: {
			// do nothing
			break;
		}
		case MouseButton::Left: {
			released = input->mouse.mouse_button_left.just_released;
			break;
		}
		case MouseButton::Middle: {
			released = input->mouse.mouse_button_middle.just_released;
			break;
		}
		case MouseButton::Right: {
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
		case MouseButton::None: {
			// do nothing
			break;
		}
		case MouseButton::Left: {
			pressed = input->mouse.mouse_button_left.just_pressed;
			break;
		}
		case MouseButton::Middle: {
			pressed = input->mouse.mouse_button_middle.just_pressed;
			break;
		}
		case MouseButton::Right: {
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
