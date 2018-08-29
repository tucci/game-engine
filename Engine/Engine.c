#pragma once

#include "Engine.h"
#include "Common/common_macros.h"

#include "Core/Game.h"



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
		debug_print("Name %s, SDl_Error %s", name, error);
	}

}

static bool push_to_event_queue(Engine* engine, Event event) {

	if (engine->event_count > MAX_EVENTS - 1) {
		debug_print("Event queue full");
		return false;
	}
	engine->event_queue[engine->event_count] = event;
	engine->event_count++;
	return true;
}

static void process_event_queue(Engine* engine) {
	for (int i = 0; i < engine->event_count; i++) {
		Event event = engine->event_queue[i];
		

		
		switch (event.kind) {

			case EventKind_Key_Down:{
				debug_print("Key Down: %d\n", event.event.key_event.key);
				update_button_state(&engine->input.keys[event.event.key_event.key], true);
				
				break;
			}

			case EventKind_Key_Up: {
				debug_print("Key Up: %d\n", event.event.key_event.key);
				update_button_state(&engine->input.keys[event.event.key_event.key], false);
				break;
			}
			case EventKind_Mouse_Button_Down: {
				debug_print("Mouse Button Down: Button:%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);

				switch (event.event.mouse_button_event.button) {
					case MouseButton_Left:
						update_button_state(&engine->input.mouse.mouse_button_left, true);
						break;
					case MouseButton_Right:
						update_button_state(&engine->input.mouse.mouse_button_right, true);
						break;
					case MouseButton_Middle:
						update_button_state(&engine->input.mouse.mouse_button_middle, true);
						break;
					default:
						break;
				}
				engine->input.mouse.pos = event.event.mouse_button_event.pos;
				
				
				
				break;
			}
			case EventKind_Mouse_Button_Up: {
				debug_print("Mouse Button Up: Button%d\tPos:<%d,%d>\n",
					event.event.mouse_button_event.button,
					event.event.mouse_button_event.pos.x,
					event.event.mouse_button_event.pos.y);

				switch (event.event.mouse_button_event.button) {
					case MouseButton_Left:
						update_button_state(&engine->input.mouse.mouse_button_left, false);
						break;
					case MouseButton_Right:
						update_button_state(&engine->input.mouse.mouse_button_right, false);
						break;
					case MouseButton_Middle:
						update_button_state(&engine->input.mouse.mouse_button_middle, false);
						break;
					default:
						break;
				}
				engine->input.mouse.pos = event.event.mouse_button_event.pos;

				break;
			}
			case EventKind_Mouse_Move: {
				


				
				//engine->input.mouse.delta_pos = cast(Vec2i){ event.event.mouse_move_event.pos.x - engine->input.mouse.pos.x, event.event.mouse_move_event.pos.y - engine->input.mouse.pos.y};
				engine->input.mouse.pos = event.event.mouse_move_event.pos;
				engine->input.mouse.delta_pos = event.event.mouse_move_event.delta_pos;

				/*debug_print("Mouse move: Pos<%d,%d>\tDelta<%d,%d>\n",
					engine->input.mouse.pos.x,
					engine->input.mouse.pos.y,
					engine->input.mouse.delta_pos.x,
					engine->input.mouse.delta_pos.y);*/

				break;
			}

			case EventKind_Window_Moved: {
				debug_print("Window move: Pos<%d,%d>\n",
					event.event.window_event.data.data1,
					event.event.window_event.data.data2);

				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.pos.x = event.event.window_event.data.data1;
				engine->window.pos.y = event.event.window_event.data.data2;
				break;
			}

			case EventKind_Window_Resized: {
				debug_print("Window resized: winId: %d, Size<%d,%d>\n",
					event.event.window_event.data.window_id,
					event.event.window_event.data.data1,
					event.event.window_event.data.data2);

				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Minimized;
				engine->window.flags &= ~WindowFlag_Maximized;
				engine->window.size.x = event.event.window_event.data.data1;
				engine->window.size.y = event.event.window_event.data.data2;
				break;
			}

			case EventKind_Window_Minimized: {
				debug_print("Window minimized, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Minimized;
				engine->window.flags &= ~WindowFlag_Maximized;
				break;
			}

			case EventKind_Window_Maximized: {
				debug_print("Window maximized, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Minimized;
				engine->window.flags |= WindowFlag_Maximized;
				break;
			}

			case EventKind_Window_Restored: {
				debug_print("Window restored, winId:%d\n",
					event.event.window_event.data.window_id
				);
				break;
			}

			case EventKind_Window_Shown: {
				debug_print("Window shown, winId:%d\n",
					event.event.window_event.data.window_id
				);
				break;
			}

			case EventKind_Window_Hidden: {
				debug_print("Window hidden, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Hidden;
				break;
			}
			case EventKind_Window_Enter_Mouse_Focus: {
				debug_print("Window enter mouse focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Has_Mouse_Focus;
				break;
			}

			case EventKind_Window_Lose_Mouse_Focus: {
				debug_print("Window lose mouse focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Has_Mouse_Focus;
				break;
			}
			case EventKind_Window_Enter_Keyboard_Focus: {
				debug_print("Window enter keyboard focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Has_Keyboard_Focus;
				break;
			}
			case EventKind_Window_Lose_Keyboard_Focus: {
				debug_print("Window lose keyboard focus, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Has_Keyboard_Focus;
				break;
			}
			default:
				break;
		}
	}

	// Clear the event queue
	engine->event_count = 0;
}


static bool init_engine_memory(Engine* engine) {
	// 
	size_t size = MEGABYTES(100);

	// Align the memory size to 64 bits
	// TODO: should this depend on the machine? 32bits vs 64bits
	int alignment = SDL_GetCPUCacheLineSize();
	engine->engine_memory_size = ALIGN_UP(size, alignment);
	engine->engine_memory = malloc(engine->engine_memory_size);
	// Set initial partition ptr to the start of the memory
	engine->partition_ptr = engine->engine_memory;

	if (engine->engine_memory == NULL) {
		// Memory failiure
		return false;
	} else {
		// TODO: do we want to zero out the memory at start

		return true;
	}
	
}

// TODO: move to a more general purpose allocator?
MemoryEnginePartition give_memory_partition(Engine* engine, size_t size) {
	MemoryEnginePartition parition;
	size_t aligned_size = ALIGN_UP(size, 64);
	parition.partition_size = aligned_size;
	parition.start_ptr = engine->partition_ptr;
	// Increment ptr for next partition
	engine->partition_ptr = (cast(char*)engine->partition_ptr) + aligned_size;
	return parition;
}

static bool init_display(Engine* engine) {
	// Grab the dpi from the display
	float dpi;
	if (SDL_GetDisplayDPI(0, &dpi, NULL, NULL) != 0) {
		log_error("SDL_GetDisplayDPI");
		return false;
	}
	assert(dpi > 0);
	engine->display.dpi = dpi;


	SDL_DisplayMode mode;
	if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
		log_error("SDL_GetCurrentDisplayMode");
		return false;
	}

	assert(mode.w > 0);
	assert(mode.h > 0);
	assert(mode.refresh_rate > 0);

	// Grab data about the display
	engine->display.refresh_rate = mode.refresh_rate;
	engine->display.w = mode.w;
	engine->display.h = mode.h;


	return true;
}


static bool init_window(Engine* engine) {

	SDL_WindowFlags sdl_flags = 0;
	engine->window.flags = 0;

	engine->window.flags |= WindowFlag_Resizable;
	//engine->window.flags |= WindowFlag_Fullscreen;



	if (engine->renderer.type == BackenedRenderer_OpenGL) {
		engine->window.flags |= WindowFlag_OpenGL;
		sdl_flags |= SDL_WINDOW_OPENGL;
	}
	
	if (engine->window.flags & WindowFlag_Resizable) {
		sdl_flags |= SDL_WINDOW_RESIZABLE;
	}
	if (engine->window.flags & WindowFlag_Fullscreen) {
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

	int w = DEFAULT_WINDOW_SIZE_X;
	int h = DEFAULT_WINDOW_SIZE_Y;

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

	engine->window.title = title;
	engine->window.pos = (Vec2i) { x, y };
	engine->window.size = (Vec2i) { w, h };
	engine->window.sdl_window = sdl_window;
	engine->window.sdl_window_flags = sdl_flags;

	

	return true;
}

static bool init_renderer(Engine* engine) {
	switch (engine->renderer.type) {
		case BackenedRenderer_Software: {
			MemoryEnginePartition parition_start = give_memory_partition(engine, MEGABYTES(10));
			return init_software_renderer(engine->window.sdl_window, &engine->renderer.software_renderer, engine->window.size, parition_start.start_ptr, parition_start.partition_size);
			break;
		}

		case BackenedRenderer_OpenGL: {
			MemoryEnginePartition parition_start = give_memory_partition(engine, MEGABYTES(10));
			return init_opengl_renderer(engine->window.sdl_window, &engine->renderer.opengl, parition_start.start_ptr, parition_start.partition_size);
			break;
		}
		default:
			break;
	}

	return true;
	
}




static bool init_keys(Engine* engine) {
	// TODO: we should expose these to the game, to see if they want to toggle these
	SDL_SetRelativeMouseMode(true);
	SDL_CaptureMouse(true);
	// Init mouse buttons
	reset_button_state(&engine->input.mouse.mouse_button_left);
	reset_button_state(&engine->input.mouse.mouse_button_middle);
	reset_button_state(&engine->input.mouse.mouse_button_right);

	for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
		reset_button_state(&engine->input.keys[c]);
	}

	return true;
}

static bool init_event_queue(Engine* engine) {
	engine->event_count = 0;
	return true;
}



static bool init_clock(Engine* engine) {

	engine->clock.ticks_per_sec = SDL_GetPerformanceFrequency();
	engine->clock.sdl_start_ticks = SDL_GetPerformanceCounter();
	engine->clock.ticks = 0;
	engine->clock.delta_ticks = 0;
	engine->clock.seconds = 0;
	engine->clock.milliseconds = 0;
	engine->clock.delta_seconds = 0;
	engine->clock.delta_milliseconds = 0;
	return true;
}

static bool init_game_loop(Engine* engine) {
	engine->game_loop.cap_framerate = false;
	//engine->game_loop.target_fps = engine->display.refresh_rate;
	engine->game_loop.target_fps = 60;
	engine->game_loop.fps = 0;
	engine->game_loop.target_delta_time = 1.0f / engine->game_loop.target_fps;
	engine->game_loop.delta_time = 0;

	// NOTE: the time step is the physics time step.
	// The time step isnt really tied to the gamer render fps
	// Right now the time step is fixed to the target fps
	engine->game_loop.time_step = 1.0f / engine->game_loop.target_fps;
	engine->game_loop.current_time = SDL_GetTicks() / 1000.0f;
	engine->game_loop.accumulator = 0;
	engine->game_loop.physics_time = 0;
	engine->game_loop.max_delta = 0.05f;
	engine->game_loop.frame_count = 0;
	return true;
}

static bool init_debug(Engine* engine) {
	return true;
}

static void update_clock(Engine* engine) {
	uint64_t ticks = SDL_GetPerformanceCounter() - engine->clock.sdl_start_ticks;
	engine->clock.delta_ticks = (int)(ticks - engine->clock.ticks);
	engine->clock.ticks = ticks;

	engine->clock.milliseconds = (engine->clock.ticks * 1000) / engine->clock.ticks_per_sec;
	engine->clock.seconds = (double)(engine->clock.ticks) / (double)(engine->clock.ticks_per_sec);

	engine->clock.delta_milliseconds = (int)((engine->clock.delta_ticks * 1000) / engine->clock.ticks_per_sec);
	engine->clock.delta_seconds = (float)(engine->clock.delta_ticks) / (float)(engine->clock.ticks_per_sec);

	/* ("Seconds: %lf\tdelta_seconds: %f\tms: %d\tdelta_ms: %d\n",
	engine->time.seconds,
	engine->time.delta_seconds,
	engine->time.milliseconds,
	engine->time.delta_milliseconds);*/

	

}

static void process_inputs(Engine* engine) {
	post_update_button_state(&engine->input.mouse.mouse_button_left);
	post_update_button_state(&engine->input.mouse.mouse_button_middle);
	post_update_button_state(&engine->input.mouse.mouse_button_right);

	for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
		post_update_button_state(&engine->input.keys[c]);
	}

	// Handle mouse seperatly
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		Event event;
		event.kind = EventKind_Mouse_Move;
		event.event.mouse_move_event.pos = (Vec2i) { x, y };
		event.event.mouse_move_event.delta_pos  = cast(Vec2i) { x - engine->input.mouse.pos.x, y - engine->input.mouse.pos.y };

		
		// if last delta == 0 && new delta  == 0, skip. dont send events if the mouse hasnt moved since last time we polled
		if ((engine->input.mouse.delta_pos.x == 0 && engine->input.mouse.delta_pos.y == 0)
			&& (event.event.mouse_move_event.delta_pos.x == 0 && event.event.mouse_move_event.delta_pos.y == 0)) {
			// Do nothing. We could flip the condition, but this reads more clear
		} else {
			push_to_event_queue(engine, event);
		}
		
	}
	
	
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event)) {

		switch (sdl_event.type) {
			// This mouse events dont capture when mouse deltas go to zero
			/*case SDL_MOUSEMOTION: {
				Event event;
				event.kind = EventKind_Mouse_Move;
				event.event.mouse_move_event.pos = (Vec2i) { sdl_event.motion.x, sdl_event.motion.y };;
				event.event.mouse_move_event.delta_pos = (Vec2i) { sdl_event.motion.xrel, sdl_event.motion.yrel };
				push_to_event_queue(engine, event);
				break;
			}*/

			case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: {

				MouseButton button;
				button = MouseButton_None;

				if (sdl_event.button.button == SDL_BUTTON_LEFT) {
					//update_button_state(&engine->mouse.mouse_button_left, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Left;
				} else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) {
					//update_button_state(&engine->mouse.mouse_button_middle, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Middle;
				} else if (sdl_event.button.button == SDL_BUTTON_RIGHT) {
					//update_button_state(&engine->mouse.mouse_button_right, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton_Right;
				}

				if (button != MouseButton_None) {
					Event event;
					event.kind = sdl_event.type == SDL_MOUSEBUTTONDOWN ? EventKind_Mouse_Button_Down : EventKind_Mouse_Button_Up;
					event.event.mouse_button_event.button = button;
					event.event.mouse_button_event.pos = (Vec2i) { .x = sdl_event.button.x, .y = sdl_event.button.y };
					push_to_event_queue(engine, event);
				}

				break;
			}

			case SDL_KEYDOWN: case SDL_KEYUP: {
				SDL_Scancode keycode = sdl_event.key.keysym.scancode;

				if (keycode) {
					Event event;
					event.kind = sdl_event.type == SDL_KEYDOWN ? EventKind_Key_Down : EventKind_Key_Up;
					event.event.key_event.key = keycode;
					push_to_event_queue(engine, event);

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
				push_to_event_queue(engine, event);


				break;
			}

			case SDL_QUIT: {
				engine->quit = true;
				break;
			}

		}
	}
}

static bool load_game(Engine* engine, const char* game_file) {


	// TODO: read game/project file
	debug_print("Loading Game\n");
	

	// Init some memory for our game
	// Load scene, get how much potential memory we would need from the scene
	MemoryEnginePartition game_partition = give_memory_partition(engine, MEGABYTES(50));


	// Bootstrap loading
	// Load game into it's own memory partition
	engine->loaded_game = cast(Game*) game_partition.start_ptr;
	linear_init(&engine->loaded_game->game_memory, game_partition.start_ptr, game_partition.partition_size);
	// Alloc the game object size so we dont overwrite the loaded game struct
	linear_alloc(&engine->loaded_game->game_memory, sizeof(Game), 4);

	// Attach and expose our engine subsystems to the game
	EngineAPI api = {
		&engine->display,
		&engine->window,
		&engine->input,
		&engine->game_loop,
	};

	attach_engine_subsytems(engine->loaded_game, api);

	// load all resources, static meshes from files
	debug_print("Loading scene objects\n");
	load_scene(engine->loaded_game, 1);

	switch (engine->renderer.type) {

		case BackenedRenderer_OpenGL: 
			set_scene_for_opengl_renderer(&engine->renderer.opengl, engine->loaded_game->loaded_scene);
			break;

		case BackenedRenderer_Software:
			set_scene_for_software_renderer(&engine->renderer.software_renderer, engine->loaded_game->loaded_scene);
			break;
		default:
			break;
	}
	

	


	debug_print("Starting Game\n");



	return true;
}

bool init_engine(Engine* engine) {

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		debug_print("Could not init SDL: %s \n", SDL_GetError());
		exit(-1);
	}

	if (!init_engine_memory(engine)) {
		debug_print("Engine failed to allocate memory at startup");
		return false;
	}


#if DEBUG
	if (!init_debug(engine)) { return false; }
#endif

	engine->quit = false;
	engine->platform = SDL_GetPlatform();
	if (!init_display(engine)) { return false; }
	if (!init_window(engine)) { return false; }
	if (!init_renderer(engine)) { return false; }
	if (!init_keys(engine)) { return false; }
	if (!init_event_queue(engine)) { return false; }
	if (!init_clock(engine)) { return false; }
	if (!init_game_loop(engine)) { return false; }

	if (!load_game(engine, "mygame.gamefile")) { return false; }
	



	return true;
}

bool destroy_engine(Engine* engine) {

	// TODO: this should be somewherer else. example unload game function?
	unload_scene(engine->loaded_game, engine->loaded_game->loaded_scene);
	 
	switch (engine->renderer.type) {
		case BackenedRenderer_Software:
			destroy_software_renderer(&engine->renderer.software_renderer);
			break;

		case BackenedRenderer_OpenGL:
			destroy_opengl_renderer(&engine->renderer.opengl);
			break;
		default:
			break;
	}
	
	
	SDL_DestroyWindow(engine->window.sdl_window);
	SDL_Quit();

	// NOTE: this frees the entire game engine memory and all the subsystems
	free(engine->engine_memory);

	return true;
}


static void update_engine_state(Engine* engine, float delta_time) {
	
	// Engine specific updates
	if (engine->input.keys[SDL_SCANCODE_G].just_pressed) {
		engine->renderer.opengl.show_debug_grid = !engine->renderer.opengl.show_debug_grid;
	}

	if (engine->input.keys[SDL_SCANCODE_H].just_pressed) {
		engine->renderer.opengl.show_debug_axes = !engine->renderer.opengl.show_debug_axes;
	}

	if (engine->input.keys[SDL_SCANCODE_L].just_pressed) {
		engine->renderer.opengl.draw_lines = !engine->renderer.opengl.draw_lines;
	}

	if (engine->input.keys[SDL_SCANCODE_ESCAPE].just_pressed) {
		engine->quit = true;
	}

	

	// Game specific update
	game_update(engine->loaded_game, &engine->input, &engine->game_loop);

	
}

static void fixed_physics_update(Engine* engine, float fixed_time) {
	// Physics stuff
}

void game_loop(Engine* engine) {
	while (!engine->quit) {
		update_clock(engine);
		// Fixed time step, with an accumulator, and max delta time interpolation
		float new_time = (SDL_GetTicks() / 1000.0f);
		float delta_time = new_time - engine->game_loop.current_time;
		engine->game_loop.delta_time = delta_time;

		
		

		/*if (delta_time >= engine->game_loop.max_delta) {
			delta_time = engine->game_loop.max_delta;
		}*/

		engine->game_loop.accumulator += delta_time;
	
		// Process inputs once per frame
		process_inputs(engine);
		process_event_queue(engine);

		

		while (engine->game_loop.accumulator >= engine->game_loop.time_step) {
			// fixed update zero or more times per frame
			fixed_physics_update(engine, engine->game_loop.time_step);
			engine->game_loop.physics_time += engine->game_loop.time_step;
			engine->game_loop.accumulator -= engine->game_loop.time_step;
		}

		// Update once per frame
		update_engine_state(engine, delta_time);

	
		// TODO: this is used to interpolate game state during the rendering
		float alpha = engine->game_loop.accumulator / engine->game_loop.time_step;


		// TODO: cull scene geometry here, do any culling and vertex processing here. Eventually thread the renderer
		
		switch (engine->renderer.type) {
			case BackenedRenderer_Software: {
				software_render(&engine->renderer.software_renderer);
				software_debug_render(&engine->renderer.software_renderer);
				software_swap_buffer(&engine->renderer.software_renderer);
				break;
			}

			case BackenedRenderer_OpenGL: {
				opengl_render(&engine->renderer.opengl, engine->window.size, true);
				opengl_swap_buffer(&engine->renderer.opengl);
				break;
			}
			default:
				break;
		}


		engine->game_loop.frame_count++;
		engine->game_loop.fps = (int) (1 / delta_time);
		engine->game_loop.current_time = new_time;

		//debug_print("FPS %d, dt %f\t", engine->game_loop.fps, delta_time);
		//debug_print("total time %f, physics time %f\n", engine->game_loop.current_time, engine->game_loop.physics_time);



		if (engine->game_loop.cap_framerate) {
			// TODO: if we have low frame rate, we might be skipping events. A possible  way to do this, is to have the input and rendering on seperate threads
			// https://gamedev.stackexchange.com/questions/90762/taking-advantage-of-multithreading-between-game-loop-and-opengl
			// TODO: look up command buffers for our renderer
			// As of right now, this frame cengineing is more of a test
			// TODO: we're gonna need debug info overlays for in game stuff
			float frame_time = (SDL_GetTicks() / 1000.0f) - new_time;
			// Frame cengineing
			if (frame_time < engine->game_loop.target_delta_time) {
				float sleep_for = engine->game_loop.target_delta_time - frame_time;
				// NOTE: since we can't change the os scheduling, we'll wait at least sleep_for time.
				// If there is a way to change the granularity of the sleep, then we should do it
				SDL_Delay((Uint32)(1000 * sleep_for));
			}
		}
		

	}
}

