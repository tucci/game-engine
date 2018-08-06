#pragma once

#include "SDLApp.h"

static void update_button_state(ButtonState* button_state, bool down) {
	bool was_down = button_state->down;
	button_state->down = down;
	button_state->pressed = down && !was_down;
	button_state->released = !down && was_down;
}

static void reset_button_state(ButtonState* button_state) {
	button_state->down = 0;
	button_state->pressed = 0;
	button_state->released = 0;
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

			case EventKind_Key_Down: {
				switch (event.event.key_event.key) {
					case SDL_SCANCODE_W: {
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Forward);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Forward);
						break;
					}
					case SDL_SCANCODE_S: {
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Backward);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Backward);
						break;
					}

					case SDL_SCANCODE_UP: {
						
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Up);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Up);
						break;
					}
					case SDL_SCANCODE_DOWN: {
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Down);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Down);
						break;
					}

					case SDL_SCANCODE_E: {
						//app->renderer.software_renderer.camera.rotation.y += 1;
						app->renderer.opengl.mainCamera.rotation.y += 1;
						break;
					}
					case SDL_SCANCODE_Q: {
						//app->renderer.software_renderer.camera.rotation.y -= 1;
						app->renderer.opengl.mainCamera.rotation.y -= 1;
						break;
					}

					case SDL_SCANCODE_RIGHT: {
						//app->renderer.software_renderer.camera.rotation.z += 1;
						app->renderer.opengl.mainCamera.rotation.z += 1;
						break;
					}
					case SDL_SCANCODE_LEFT: {
						//app->renderer.software_renderer.camera.rotation.z -= 1;
						app->renderer.opengl.mainCamera.rotation.z -= 1;
						break;
					}

					case SDL_SCANCODE_A: {
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Left);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Left);
						break;
					}
					case SDL_SCANCODE_D: {
						//app->renderer.software_renderer.camera.pos.xyz_ = vec_add(app->renderer.software_renderer.camera.pos.xyz_, Vec3f_Right);
						app->renderer.opengl.mainCamera.pos.xyz = vec_add(app->renderer.opengl.mainCamera.pos.xyz, Vec3f_Right);
						break;
					}
					case SDL_SCANCODE_ESCAPE: {
						app->quit = true;
						break;
					}
					default:
						break;
				}

				event_printf("Key Down: %d\n", event.event.key_event.key);
				break;
			}
			case EventKind_Key_Up: {
				event_printf("Key Up: %d\n", event.event.key_event.key);
				break;
			}
			case EventKind_Mouse_Button_Down: {
				event_printf("Mouse Button Down: Button:%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);
				break;
			}
			case EventKind_Mouse_Button_Up: {
				event_printf("Mouse Button Up: Button%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);
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

static bool init_time(App* app) {
	app->time.ticks_per_sec = SDL_GetPerformanceFrequency();
	app->time.sdl_start_ticks = SDL_GetPerformanceCounter();
	app->time.ticks = 0;
	app->time.delta_ticks = 0;
	app->time.seconds = 0;
	app->time.milliseconds = 0;
	app->time.delta_seconds = 0;
	app->time.delta_milliseconds = 0;
	return true;
}

static bool init_debug(App* app) {
	app->debug.print_events = DEBUG_ALLOW_PRINT_EVENT;
	return true;
}

static void update_time(App* app) {
	uint64_t ticks = SDL_GetPerformanceCounter() - app->time.sdl_start_ticks;
	app->time.delta_ticks = (int)(ticks - app->time.ticks);
	app->time.ticks = ticks;

	app->time.milliseconds = (app->time.ticks * 1000) / app->time.ticks_per_sec;
	app->time.seconds = (double)(app->time.ticks) / (double)(app->time.ticks_per_sec);

	app->time.delta_milliseconds = (int)((app->time.delta_ticks * 1000) / app->time.ticks_per_sec);
	app->time.delta_seconds = (float)(app->time.delta_ticks) / (float)(app->time.ticks_per_sec);

	/*printf("Seconds: %lf\tdelta_seconds: %f\tms: %d\tdelta_ms: %d\n",
	app->time.seconds,
	app->time.delta_seconds,
	app->time.milliseconds,
	app->time.delta_milliseconds);*/

	/*app.time.nsecs = (app.time.ticks * 1000 * 1000 * 1000) / app.time.ticks_per_sec;
	app.time.usecs = (app.time.ticks * 1000 * 1000) / app.time.ticks_per_sec;*/
	/*app.time.delta_nsecs = int((app.time.delta_ticks * 1000 * 1000 * 1000) / app.time.ticks_per_sec);
	app.time.delta_usecs = int((app.time.delta_ticks * 1000 * 1000) / app.time.ticks_per_sec);*/

}

static void process_inputs(App* app) {

	// TODO: need to handle events when multiple keys are presses at the same time. right now when two keys are pressed at the same time, only one is sent to the event queue
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event)) {
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

				// TODO: (steven), see if we could handle this only through the event queue, and not with update button state
				if (sdl_event.button.button == SDL_BUTTON_LEFT) {
					update_button_state(&app->mouse.mouse_button_left, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Left;
				} else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) {
					update_button_state(&app->mouse.mouse_button_middle, sdl_event.button.state == SDL_PRESSED);
					//update_digital_button(&app.mouse.middle_button, event.button.state == SDL_PRESSED);
					button = MouseButton_Middle;
				} else if (sdl_event.button.button == SDL_BUTTON_RIGHT) {
					update_button_state(&app->mouse.mouse_button_right, sdl_event.button.state == SDL_PRESSED);
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
					// TODO(steven) see if we can handle this through the event queue
					update_button_state(&app->keys[keycode], sdl_event.key.state == SDL_PRESSED);

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
	if (!init_time(app)) { return false; }



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

void game_loop(App* app) {
	while (!app->quit) {
		process_inputs(app);
		process_event_queue(app);

		switch (app->renderer.type) {
			case BackenedRenderer_Software: {
				software_render(&app->renderer.software_renderer);
				software_debug_render(&app->renderer.software_renderer);
				break;
			}

			case BackenedRenderer_OpenGL: {
				opengl_render(&app->renderer.opengl);
				break;
			}
			default:
				break;
		}

		update_time(app);

	}
}
