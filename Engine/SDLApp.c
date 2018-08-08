#pragma once

#include "SDLApp.h"


static void update_button_state(ButtonState* button_state, bool down_now) {
	// Update for edge events
	bool was_down = button_state->down;
	button_state->down = down_now;
	button_state->just_pressed = down_now && !was_down;
	button_state->just_released = !down_now && was_down;
}

static void reset_button_state(ButtonState* button_state) {
	button_state->down = 0;
	button_state->just_pressed = 0;
	button_state->just_released = 0;
}

static void post_update_button_state(ButtonState* button_state) {
	// After the button was released, we'll need to reset all the button states
	if (button_state->just_released) {
		reset_button_state(button_state);
	}

	// If the button is down and already pressed, then it no longer was just pressed
	// just_pressed should stay true for one frame
	if (button_state->just_pressed && button_state->down) {
		button_state->just_pressed = false;
	}
}


// TODO: create a error buffer, with a dump to file 
static void log_error(const char* name) {
	const char* error = SDL_GetError();
	if (*error) {
		printf("Name %s, SDl_Error %s", name, error);
	}

}

static bool push_to_event_queue(App* app, Event event) {

	if (app->event_count > MAX_EVENTS - 1) {
		printf("Event queue full");
		return false;
	}
	app->event_queue[app->event_count] = event;
	app->event_count++;
	return true;
}

static void process_event_queue(App* app) {
	for (int i = 0; i < app->event_count; i++) {
		Event event = app->event_queue[i];
		

		
		switch (event.kind) {

			case EventKind_Key_Down:{
				event_printf("Key Down: %d\n", event.event.key_event.key);
				update_button_state(&app->keys[event.event.key_event.key], true);
				
				break;
			}

			case EventKind_Key_Up: {
				event_printf("Key Up: %d\n", event.event.key_event.key);
				update_button_state(&app->keys[event.event.key_event.key], false);
				break;
			}
			case EventKind_Mouse_Button_Down: {
				event_printf("Mouse Button Down: Button:%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);

				switch (event.event.mouse_button_event.button) {
					case MouseButton_Left:
						update_button_state(&app->mouse.mouse_button_left, true);
						break;
					case MouseButton_Right:
						update_button_state(&app->mouse.mouse_button_right, true);
						break;
					case MouseButton_Middle:
						update_button_state(&app->mouse.mouse_button_middle, true);
						break;
					default:
						break;
				}
				app->mouse.pos = event.event.mouse_button_event.pos;
				
				
				
				break;
			}
			case EventKind_Mouse_Button_Up: {
				event_printf("Mouse Button Up: Button%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);

				switch (event.event.mouse_button_event.button) {
					case MouseButton_Left:
						update_button_state(&app->mouse.mouse_button_left, false);
						break;
					case MouseButton_Right:
						update_button_state(&app->mouse.mouse_button_right, false);
						break;
					case MouseButton_Middle:
						update_button_state(&app->mouse.mouse_button_middle, false);
						break;
					default:
						break;
				}
				app->mouse.pos = event.event.mouse_button_event.pos;

				break;
			}
			case EventKind_Mouse_Move: {
				/*event_printf("Mouse move: Pos<%d,%d>\tDelta<%d,%d>\n",
				event.event.mouse_move_event.pos.x,
				event.event.mouse_move_event.pos.y,
				event.event.mouse_move_event.delta_pos.x,
				event.event.mouse_move_event.delta_pos.y);*/

				app->mouse.pos = event.event.mouse_move_event.pos;
				app->mouse.delta_pos = event.event.mouse_move_event.delta_pos;

				break;
			}

			case EventKind_Window_Moved: {
				event_printf("Window move: Pos<%d,%d>\n",
					event.event.window_event.data.data1,
					event.event.window_event.data.data2);

				app->window.window_id = event.event.window_event.data.window_id;
				app->window.pos.x = event.event.window_event.data.data1;
				app->window.pos.y = event.event.window_event.data.data2;
				break;
			}

			case EventKind_Window_Resized: {
				event_printf("Window resized: winId: %d, Size<%d,%d>\n",
					event.event.window_event.data.window_id,
					event.event.window_event.data.data1,
					event.event.window_event.data.data2);

				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags &= ~WindowFlag_Minimized;
				app->window.flags &= ~WindowFlag_Maximized;
				app->window.size.x = event.event.window_event.data.data1;
				app->window.size.y = event.event.window_event.data.data2;
				break;
			}

			case EventKind_Window_Minimized: {
				event_printf("Window minimized, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags |= WindowFlag_Minimized;
				app->window.flags &= ~WindowFlag_Maximized;
				break;
			}

			case EventKind_Window_Maximized: {
				event_printf("Window maximized, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags &= ~WindowFlag_Minimized;
				app->window.flags |= WindowFlag_Maximized;
				break;
			}

			case EventKind_Window_Restored: {
				event_printf("Window restored, winId:%d\n",
					event.event.window_event.data.window_id
				);
				break;
			}

			case EventKind_Window_Shown: {
				event_printf("Window shown, winId:%d\n",
					event.event.window_event.data.window_id
				);
				break;
			}

			case EventKind_Window_Hidden: {
				event_printf("Window hidden, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags |= WindowFlag_Hidden;
				break;
			}
			case EventKind_Window_Enter_Mouse_Focus: {
				event_printf("Window enter mouse focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags |= WindowFlag_Has_Mouse_Focus;
				break;
			}

			case EventKind_Window_Lose_Mouse_Focus: {
				event_printf("Window lose mouse focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags &= ~WindowFlag_Has_Mouse_Focus;
				break;
			}
			case EventKind_Window_Enter_Keyboard_Focus: {
				event_printf("Window enter keyboard focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags |= WindowFlag_Has_Keyboard_Focus;
				break;
			}
			case EventKind_Window_Lose_Keyboard_Focus: {
				event_printf("Window lose keyboard focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				app->window.window_id = event.event.window_event.data.window_id;
				app->window.flags &= ~WindowFlag_Has_Keyboard_Focus;
				break;
			}
			default:
				break;
		}
	}

	// Clear the event queue
	app->event_count = 0;
}

static bool init_display(App* app) {
	// Grab the dpi from the display
	float dpi;
	if (SDL_GetDisplayDPI(0, &dpi, NULL, NULL) != 0) {
		log_error("SDL_GetDisplayDPI");
		return false;
	}
	assert(dpi > 0);
	app->display.dpi = dpi;


	SDL_DisplayMode mode;
	if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
		log_error("SDL_GetCurrentDisplayMode");
		return false;
	}

	assert(mode.w > 0);
	assert(mode.h > 0);
	assert(mode.refresh_rate > 0);

	// Grab data about the display
	app->display.refresh_rate = mode.refresh_rate;
	app->display.w = mode.w;
	app->display.h = mode.h;


	return true;
}


static bool init_window(App* app) {

	SDL_WindowFlags sdl_flags = 0;
	app->window.flags = 0;

	app->window.flags |= WindowFlag_Resizable;
	//app->window.flags |= WindowFlag_Fullscreen;



	if (app->renderer.type == BackenedRenderer_OpenGL) {
		app->window.flags |= WindowFlag_OpenGL;
		sdl_flags |= SDL_WINDOW_OPENGL;
	}
	
	if (app->window.flags & WindowFlag_Resizable) {
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}
	if (app->window.flags & WindowFlag_Fullscreen) {
		sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}






	SDL_Window* sdl_window;

	const char* title = WINDOW_TITLE;
	// by setting the x,y at SDL_WINDOWPOS_CENTERED, it doesnt give the actual x,y pos values. 
	// SDL_WINDOWPOS_CENTERED is a flag used by SDL_CreateWindow to set the actual pos values
	// Once SDL_CreateWindow is called, we can get the x,y pos by calling SDL_GetWindowPosition
	// Note these are not the actual pos values yet
	int x = SDL_WINDOWPOS_CENTERED;
	int y = SDL_WINDOWPOS_CENTERED;

	int w = WINDOW_SIZE_X;
	int h = WINDOW_SIZE_Y;

	sdl_window = SDL_CreateWindow(
		title,
		x, y,
		w, h,
		sdl_flags
	);

	// Now the actual pos values can be set by calling SDL_GetWindowPosition
	SDL_GetWindowPosition(sdl_window, &x, &y);
	assert(x > 0 && x != SDL_WINDOWPOS_CENTERED);
	assert(y > 0 && y != SDL_WINDOWPOS_CENTERED);



	if (!sdl_window) {
		log_error("Could not create window");

		return false;
	}

	app->window.title = title;
	app->window.pos = (Vec2i) { x, y };
	app->window.size = (Vec2i) { w, h };
	app->window.sdl_window = sdl_window;
	app->window.sdl_window_flags = sdl_flags;

	

	return true;
}

static bool init_renderer(App* app) {
	switch (app->renderer.type) {
		case BackenedRenderer_Software: {
			init_software_renderer(app->window.sdl_window, &app->renderer.software_renderer, app->window.size);
			break;
		}

		case BackenedRenderer_OpenGL: {
			init_opengl_renderer(app->window.sdl_window, &app->renderer.opengl, app->window.size);
			break;
		}
		default:
			break;
	}

	return true;
	
}




static bool init_keys(App* app) {
	// Init mouse buttons
	reset_button_state(&app->mouse.mouse_button_left);
	reset_button_state(&app->mouse.mouse_button_middle);
	reset_button_state(&app->mouse.mouse_button_right);

	for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
		reset_button_state(&app->keys[c]);
	}

	return true;
}

static bool init_event_queue(App* app) {
	app->event_count = 0;
	return true;
}



static bool init_clock(App* app) {

	app->clock.ticks_per_sec = SDL_GetPerformanceFrequency();
	app->clock.sdl_start_ticks = SDL_GetPerformanceCounter();
	app->clock.ticks = 0;
	app->clock.delta_ticks = 0;
	app->clock.seconds = 0;
	app->clock.milliseconds = 0;
	app->clock.delta_seconds = 0;
	app->clock.delta_milliseconds = 0;
	return true;
}

static bool init_game_loop(App* app) {
	app->game_loop.cap_framerate = false;
	//app->game_loop.target_fps = app->display.refresh_rate;
	app->game_loop.target_fps = 60;
	app->game_loop.fps = 0;
	app->game_loop.target_delta_time = 1.0f / app->game_loop.target_fps;
	app->game_loop.delta_time = 0;

	// NOTE: the time step is the physics time step.
	// The time step isnt really tied to the gamer render fps
	// Right now the time step is fixed to the target fps
	app->game_loop.time_step = 1.0f / app->game_loop.target_fps;
	app->game_loop.current_time = SDL_GetTicks() / 1000.0f;
	app->game_loop.accumulator = 0;
	app->game_loop.physics_time = 0;
	app->game_loop.max_delta = 0.05f;
	app->game_loop.frame_count = 0;
	return true;
}

static bool init_debug(App* app) {
	app->debug.print_events = DEBUG_ALLOW_PRINT_EVENT;
	return true;
}

static void update_clock(App* app) {
	uint64_t ticks = SDL_GetPerformanceCounter() - app->clock.sdl_start_ticks;
	app->clock.delta_ticks = (int)(ticks - app->clock.ticks);
	app->clock.ticks = ticks;

	app->clock.milliseconds = (app->clock.ticks * 1000) / app->clock.ticks_per_sec;
	app->clock.seconds = (double)(app->clock.ticks) / (double)(app->clock.ticks_per_sec);

	app->clock.delta_milliseconds = (int)((app->clock.delta_ticks * 1000) / app->clock.ticks_per_sec);
	app->clock.delta_seconds = (float)(app->clock.delta_ticks) / (float)(app->clock.ticks_per_sec);

	/*printf("Seconds: %lf\tdelta_seconds: %f\tms: %d\tdelta_ms: %d\n",
	app->time.seconds,
	app->time.delta_seconds,
	app->time.milliseconds,
	app->time.delta_milliseconds);*/

	

}

static void process_inputs(App* app) {
	post_update_button_state(&app->mouse.mouse_button_left);
	post_update_button_state(&app->mouse.mouse_button_middle);
	post_update_button_state(&app->mouse.mouse_button_right);

	for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
		post_update_button_state(&app->keys[c]);
	}

	

	
	


	SDL_Event sdl_event;
	
	while (SDL_PollEvent(&sdl_event)) {

		
		/*const Uint8 *keys = SDL_GetKeyboardState(NULL);
		for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
			
			Event event;
			event.event.key_event.key = i;

			ButtonState* button_state = &app->keys[i];

			update_button_state(&app->keys[i], (bool)keys[i]);

			if (button_state->pressed) {
				event.kind = EventKind_Key_Pressed;
				push_to_event_queue(app, event);
			}

			else if (button_state->down) {
				event.kind = EventKind_Key_Down;
				push_to_event_queue(app, event);
			} 
	
			if (button_state->released) {
				event.kind = EventKind_Key_Up;
				push_to_event_queue(app, event);
			}
			
		}*/
		

		switch (sdl_event.type) {
			case SDL_MOUSEMOTION: {
				Event event;
				event.kind = EventKind_Mouse_Move;
				event.event.mouse_move_event.pos = (Vec2i) { sdl_event.motion.x, sdl_event.motion.y };;
				event.event.mouse_move_event.delta_pos = (Vec2i) { sdl_event.motion.xrel, sdl_event.motion.yrel };
				push_to_event_queue(app, event);
				break;
			}

			case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: {

				MouseButton button;
				button = MouseButton_None;

				if (sdl_event.button.button == SDL_BUTTON_LEFT) {
					//update_button_state(&app->mouse.mouse_button_left, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Left;
				} else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) {
					//update_button_state(&app->mouse.mouse_button_middle, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Middle;
				} else if (sdl_event.button.button == SDL_BUTTON_RIGHT) {
					//update_button_state(&app->mouse.mouse_button_right, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Right;
				}

				if (button != MouseButton_None) {
					Event event;
					event.kind = sdl_event.type == SDL_MOUSEBUTTONDOWN ? EventKind_Mouse_Button_Down : EventKind_Mouse_Button_Up;
					event.event.mouse_button_event.button = button;
					event.event.mouse_button_event.pos = (Vec2i) { .x = sdl_event.button.x, .y = sdl_event.button.y };
					push_to_event_queue(app, event);
				}

				break;
			}

			case SDL_KEYDOWN: case SDL_KEYUP: {
				SDL_Scancode keycode = sdl_event.key.keysym.scancode;

				if (keycode) {
					Event event;
					event.kind = sdl_event.type == SDL_KEYDOWN ? EventKind_Key_Down : EventKind_Key_Up;
					event.event.key_event.key = keycode;
					push_to_event_queue(app, event);

				}
				break;
			}
			case SDL_WINDOWEVENT: {

				Event event;
				event.event.window_event.data.window_id = sdl_event.window.windowID;
				event.event.window_event.data.data1 = sdl_event.window.data1;
				event.event.window_event.data.data2 = sdl_event.window.data2;

				switch (sdl_event.window.event) {
					case SDL_WINDOWEVENT_MOVED: {event.kind = EventKind_Window_Moved;	break; }
					case SDL_WINDOWEVENT_RESIZED: {event.kind = EventKind_Window_Resized;	break; }
					case SDL_WINDOWEVENT_MINIMIZED: {event.kind = EventKind_Window_Minimized;	break; }
					case SDL_WINDOWEVENT_MAXIMIZED: {event.kind = EventKind_Window_Maximized;	break; }
					case SDL_WINDOWEVENT_RESTORED: {event.kind = EventKind_Window_Restored;	break; }
					case SDL_WINDOWEVENT_SHOWN: {event.kind = EventKind_Window_Shown;	break; }
					case SDL_WINDOWEVENT_HIDDEN: {event.kind = EventKind_Window_Hidden;	break; }
					case SDL_WINDOWEVENT_ENTER: {event.kind = EventKind_Window_Enter_Mouse_Focus;	break; }
					case SDL_WINDOWEVENT_LEAVE: {event.kind = EventKind_Window_Lose_Mouse_Focus;	break; }
					case SDL_WINDOWEVENT_FOCUS_GAINED: {event.kind = EventKind_Window_Enter_Keyboard_Focus;	break; }
					case SDL_WINDOWEVENT_FOCUS_LOST: {event.kind = EventKind_Window_Lose_Keyboard_Focus;	break; }
					default: {
						event.kind = EventKind_None;
						break;
					}

				}
				push_to_event_queue(app, event);


				break;
			}

			case SDL_QUIT: {
				app->quit = true;
				break;
			}

		}
	}
}

bool init_app(App* app) {

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		printf("Could not init SDL: %s \n", SDL_GetError());
		exit(-1);
	}


#if DEBUG
	if (!init_debug(app)) { return false; }
#endif

	app->quit = false;
	app->platform = SDL_GetPlatform();
	if (!init_display(app)) { return false; }
	if (!init_window(app)) { return false; }
	if (!init_renderer(app)) { return false; }
	if (!init_keys(app)) { return false; }
	if (!init_event_queue(app)) { return false; }
	if (!init_clock(app)) { return false; }
	if (!init_game_loop(app)) { return false; }



	return true;
}

bool destroy_app(App* app) {
	 
	switch (app->renderer.type) {
		case BackenedRenderer_Software :
			destroy_software_renderer(&app->renderer.software_renderer);
			break;

		case BackenedRenderer_OpenGL:
			destroy_opengl_renderer(&app->renderer.opengl);
			break;
		default:
			break;
	}
	
	SDL_DestroyWindow(app->window.sdl_window);
	SDL_Quit();
	return true;
}


void update(App* app, float deltaTime) {
	
	if (app->keys[SDL_SCANCODE_G].just_pressed) {		
		app->renderer.opengl.show_debug_grid = !app->renderer.opengl.show_debug_grid;
	}

	if (app->keys[SDL_SCANCODE_H].just_pressed) {
		app->renderer.opengl.show_debug_axes = !app->renderer.opengl.show_debug_axes;
	}

	if (app->keys[SDL_SCANCODE_ESCAPE].just_pressed) {
		app->quit = true;
	}


	// TODO: refactor camera to game state when we have one.
	//dont want to have camera in software renderer and gl renderer.
	// Seperate input from rendering
	if (app->keys[SDL_SCANCODE_W].down) {
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Backward));
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Backward));
	}

	if (app->keys[SDL_SCANCODE_S].down) {
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Forward));
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Forward));
	}

	if (app->keys[SDL_SCANCODE_1].down) {
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Up));
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Up));
	}

	if (app->keys[SDL_SCANCODE_2].down) {
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Down));
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Down));
	}

	if (app->keys[SDL_SCANCODE_A].down) {
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Left));
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Left));
	}

	if (app->keys[SDL_SCANCODE_D].down) {
		//app->renderer.software_renderer.camera.pos.xyz = vec_add(app->renderer.software_renderer.camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Right));
		app->renderer.opengl.main_camera.pos.xyz = vec_add(app->renderer.opengl.main_camera.pos.xyz, vec_multiply(deltaTime, Vec3f_Right));
	}

	if (app->keys[SDL_SCANCODE_E].down) {
		//app->renderer.software_renderer.camera.rotation.y += deltaTime * 100;
		app->renderer.opengl.main_camera.rotation.y += deltaTime * 100;
	}

	if (app->keys[SDL_SCANCODE_Q].down) {
		//app->renderer.software_renderer.camera.rotation.y -= deltaTime * 100;
		app->renderer.opengl.main_camera.rotation.y -= deltaTime * 100;
	}

	if (app->keys[SDL_SCANCODE_X].down) {
		app->renderer.opengl.main_camera.rotation.z += deltaTime * 100;
	}

	if (app->keys[SDL_SCANCODE_Z].down) {
		app->renderer.opengl.main_camera.rotation.z -= deltaTime * 100;
	}

	
}

void fixed_update(App* app, float fixed_time) {
	// Physics stuff
}

void game_loop(App* app) {
	// TODO: proper game loop
	while (!app->quit) {

		
		update_clock(app);

		// Fixed time step, with an accumulator, and max delta time interpolation
		double new_time = (SDL_GetTicks() / 1000.0f);
		double delta_time = new_time - app->game_loop.current_time;
		app->game_loop.delta_time = delta_time;

		
		

		/*if (delta_time >= app->game_loop.max_delta) {
			delta_time = app->game_loop.max_delta;
		}*/

		app->game_loop.accumulator += delta_time;
	
		// Process inputs once per frame
		process_inputs(app);
		process_event_queue(app);

		

		while (app->game_loop.accumulator >= app->game_loop.time_step) {
			// fixed update zero or more times per frame
			fixed_update(app, app->game_loop.time_step);
			app->game_loop.physics_time += app->game_loop.time_step;
			app->game_loop.accumulator -= app->game_loop.time_step;
		}

		// Update once per frame
		update(app, delta_time);

	
		// TODO: this is used to interpolate game state during the rendering
		float alpha = app->game_loop.accumulator / app->game_loop.time_step;

		switch (app->renderer.type) {
			case BackenedRenderer_Software: {
				software_render(&app->renderer.software_renderer);
				software_debug_render(&app->renderer.software_renderer);
				software_swap_buffer(&app->renderer.software_renderer);
				break;
			}

			case BackenedRenderer_OpenGL: {
				opengl_render(&app->renderer.opengl);
				opengl_debug_render(&app->renderer.opengl);
				opengl_swap_buffer(&app->renderer.opengl);
				break;
			}
			default:
				break;
		}


		app->game_loop.frame_count++;
		app->game_loop.fps = (int) (1 / delta_time);
		app->game_loop.current_time = new_time;


		printf("FPS %d, dt %f\t", app->game_loop.fps, delta_time);
		printf("total time %f, physics time %f\n", app->game_loop.current_time, app->game_loop.physics_time);



		if (app->game_loop.cap_framerate) {
			// TODO: if we have low frame rate, we might be skipping events. A possible  way to do this, is to have the input and rendering on seperate threads
			// https://gamedev.stackexchange.com/questions/90762/taking-advantage-of-multithreading-between-game-loop-and-opengl
			// As of right now, this frame capping is more of a test
			// TODO: we're gonna need debug info overlays for in game stuff
			double frame_time = (SDL_GetTicks() / 1000.0f) - new_time;
			// Frame capping
			if (frame_time < app->game_loop.target_delta_time) {
				float sleep_for = app->game_loop.target_delta_time - frame_time;
				// NOTE: since we can't change the os scheduling, we'll wait at least sleep_for time.
				// If there is a way to change the granularity of the sleep, then we should do it
				SDL_Delay(1000 * sleep_for);
			}
		}
		

		
		




		

	}
}

