#pragma once



#include "Core/Engine.h"
#include "Common/common_macros.h"

#include "Core/Game.h"

#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"
#include "Logger.h"

//#if ENGINE_MODE_EDITOR

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_sdl.h"
#include "../../imgui/imgui_impl_opengl3.h"
//#endif



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
            
			case EventKind::Key_Down:{
				debug_print("Key Down: %d\n", event.event.key_event.key);
				
				update_button_state(&engine->input.keys[event.event.key_event.key], true);
				
				break;
			}
            
			case EventKind::Key_Up: {
				debug_print("Key Up: %d\n", event.event.key_event.key);
				update_button_state(&engine->input.keys[event.event.key_event.key], false);
				break;
			}
			case EventKind::Mouse_Button_Down: {
				debug_print("Mouse Button Down: Button:%d\tPos:<%d,%d>\n",
                            event.event.mouse_button_event.button,
                            event.event.mouse_button_event.pos.x,
                            event.event.mouse_button_event.pos.y);
                
				switch (event.event.mouse_button_event.button) {
					case MouseButton::Left:
                    update_button_state(&engine->input.mouse.mouse_button_left, true);
                    break;
					case MouseButton::Right:
                    update_button_state(&engine->input.mouse.mouse_button_right, true);
                    break;
					case MouseButton::Middle:
                    update_button_state(&engine->input.mouse.mouse_button_middle, true);
                    break;
					case MouseButton::None:
                    break;
					default:
                    break;
				}
				engine->input.mouse.pos = event.event.mouse_button_event.pos;
				
				
				
				break;
			}
			case EventKind::Mouse_Button_Up: {
				debug_print("Mouse Button Up: Button%d\tPos:<%d,%d>\n",
                            event.event.mouse_button_event.button,
                            event.event.mouse_button_event.pos.x,
                            event.event.mouse_button_event.pos.y);
                
				switch (event.event.mouse_button_event.button) {
					case MouseButton::Left:
                    update_button_state(&engine->input.mouse.mouse_button_left, false);
                    break;
					case MouseButton::Right:
                    update_button_state(&engine->input.mouse.mouse_button_right, false);
                    break;
					case MouseButton::Middle:
                    update_button_state(&engine->input.mouse.mouse_button_middle, false);
                    break;
					default:
                    break;
				}
				engine->input.mouse.pos = event.event.mouse_button_event.pos;
                
				break;
			}
			case EventKind::Mouse_Move: {
				engine->input.mouse.pos = event.event.mouse_move_event.pos;
				engine->input.mouse.delta_pos = event.event.mouse_move_event.delta_pos;
				break;
			}
			case EventKind::Mouse_Global_Move: {
				engine->input.mouse.global_pos = event.event.mouse_move_event.global_pos;
				engine->input.mouse.global_delta_pos = event.event.mouse_move_event.global_delta_pos;
				break;
			}
			case EventKind::Mouse_Scroll: {
				engine->input.mouse.scroll = Vec2i(event.event.mouse_scroll_event.x_scroll, event.event.mouse_scroll_event.y_scroll);
				break;
			}
			case EventKind::Window_Moved: {
				debug_print("Window move: Pos<%d,%d>\n",
                            event.event.window_event.data.data1,
                            event.event.window_event.data.data2);
                
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.pos.x = event.event.window_event.data.data1;
				engine->window.pos.y = event.event.window_event.data.data2;
				break;
			}
			
            
			case EventKind::Window_Resized: {
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
            
			case EventKind::Window_Minimized: {
				debug_print("Window minimized, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Minimized;
				engine->window.flags &= ~WindowFlag_Maximized;
				break;
			}
            
			case EventKind::Window_Maximized: {
				debug_print("Window maximized, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Minimized;
				engine->window.flags |= WindowFlag_Maximized;
				break;
			}
            
			case EventKind::Window_Restored: {
				debug_print("Window restored, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				break;
			}
            
			case EventKind::Window_Shown: {
				debug_print("Window shown, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				break;
			}
            
			case EventKind::Window_Hidden: {
				debug_print("Window hidden, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Hidden;
				break;
			}

			case EventKind::Window_Closed: {
				debug_print("Window closed, winId:%d\n",
					event.event.window_event.data.window_id
				);
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->quit = true;
				break;
			}
			case EventKind::Window_Enter_Mouse_Focus: {
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Has_Mouse_Focus;
				break;
			}
            
			case EventKind::Window_Lose_Mouse_Focus: {
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Has_Mouse_Focus;
				break;
			}
			case EventKind::Window_Enter_Keyboard_Focus: {
				debug_print("Window enter keyboard focus, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags |= WindowFlag_Has_Keyboard_Focus;
				break;
			}
			case EventKind::Window_Lose_Keyboard_Focus: {
				debug_print("Window lose keyboard focus, winId:%d\n",
                            event.event.window_event.data.window_id
                            );
				engine->window.window_id = event.event.window_event.data.window_id;
				engine->window.flags &= ~WindowFlag_Has_Keyboard_Focus;
				break;
			}
			case EventKind::None: {
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
	LOG_INFO("ENGINE", "Starting up memory subsystems");
	//size_t size = ENGINE_MEMORY;
	//
	//// Align the memory size to 64 bits
	//// TODO: should this depend on the machine? 32bits vs 64bits
	//int alignment = SDL_GetCPUCacheLineSize();
	//engine->engine_memory_size = ALIGN_UP(size, alignment);
	//engine->engine_memory = malloc(engine->engine_memory_size);
	//// Set initial partition ptr to the start of the memory
	//engine->partition_ptr = engine->engine_memory;
	//
	//if (engine->engine_memory == NULL) {
	//	// Memory failiure
	//	return false;
	//} else {
	//	// TODO: do we want to zero out the memory at start
	//
    return true;
	//}
	
}



static bool init_display(Engine* engine) {
	LOG_INFO("ENGINE", "Starting up display subsystem");
	// Grab the dpi from the display
	float dpi;
	if (SDL_GetDisplayDPI(0, &dpi, NULL, NULL) != 0) {
		LOG_WARN("ENGINE", "SDL_GetDisplayDPI, SDl_Error %s", SDL_GetError());
		return false;
	}
	assert(dpi > 0);
	engine->display.dpi = dpi;
    
    
	SDL_DisplayMode mode;
	if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
		LOG_WARN("ENGINE", "SDL_GetCurrentDisplayMode, SDl_Error %s", SDL_GetError());
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
	LOG_INFO("ENGINE", "Starting up window subsystem");
	SDL_Window* sdl_window;
	
    

	

	SDL_SetHint(SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4, "1");

	int x = SDL_WINDOWPOS_CENTERED;
	int y = SDL_WINDOWPOS_CENTERED;

	int w = DEFAULT_WINDOW_SIZE_X;
	int h = DEFAULT_WINDOW_SIZE_Y;

	engine->window.flags = 0;
	engine->window.flags |= WindowFlag_Resizable;

#if ENGINE_MODE_EDITOR
	
	//engine->window.flags |= SDL_WINDOW_INPUT_GRABBED;
	//engine->window.flags |= WindowFlag_Fullscreen;
	engine->window.flags |= WindowFlag_Maximized;
	//engine->window.flags |= WindowFlag_Borderless;
#endif
	

	// TODO: game crashes when we do full screen
	//engine->window.flags |= WindowFlag_Fullscreen;
	if (engine->renderer.type == BackenedRendererType::OpenGL) {
		engine->window.flags |= WindowFlag_OpenGL;
	}


	SDL_WindowFlags sdl_flags = convert_to_sdl_flags(engine->window.flags);
	const char* title = WINDOW_TITLE;
	// by setting the x,y at SDL_WINDOWPOS_CENTERED, it doesnt give the actual x,y pos values. 
	// SDL_WINDOWPOS_CENTERED is a flag used by SDL_CreateWindow to set the actual pos values
	// Once SDL_CreateWindow is called, we can get the x,y pos by calling SDL_GetWindowPosition
	// Note these are not the actual pos values yet

	/*int x = SDL_WINDOWPOS_CENTERED;
	int y = SDL_WINDOWPOS_CENTERED;*/
	
	

	

	sdl_window = SDL_CreateWindow(
		title,
		x, y,
		w, h,
		sdl_flags
	);

	
	
	
    
	if (!sdl_window) {
		LOG_FATAL("ENGINE", "Could not create window, SDl_Error %s", SDL_GetError());
		return false;
	}
	

	engine->window.title = SDL_GetWindowTitle(sdl_window);
	SDL_GetWindowPosition(sdl_window, &engine->window.pos.x, &engine->window.pos.y);
	SDL_GetWindowSize(sdl_window, &engine->window.size.x, &engine->window.size.y);
	engine->window.sdl_window = sdl_window;
	engine->window.sdl_window_flags = (SDL_WindowFlags) SDL_GetWindowFlags(sdl_window);
    

	
	
	//SDL_SetWindowGrab(sdl_window, SDL_TRUE);

#if ENGINE_MODE_EDITOR
	//SDL_SetRelativeMouseMode(SDL_TRUE);
	//SDL_CaptureMouse(SDL_TRUE);


#else
	// TODO: this is a temp fix. we need to manually warp the mouse
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);
#endif


	return true;
}

static bool init_backend_renderer(Engine* engine) {
	LOG_INFO("ENGINE", "Starting up renderer");
	init_backend_renderer(&engine->renderer, engine->window.sdl_window);	
	return true;
}




static bool init_keys(Engine* engine) {

	
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
	LOG_INFO("ENGINE", "Starting up event subsystem");
	engine->event_count = 0;
	// We want to be able to handle SDL_SYSWMEVENT manually
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	return true;
}




static bool init_game_loop(Engine* engine) {
	GameTimer* timer = &engine->game_loop;
	timer->ticks_per_sec = SDL_GetPerformanceFrequency();
	timer->sdl_start_ticks = SDL_GetPerformanceCounter();
	timer->ticks = 0;
	timer->delta_ticks = 0;
	timer->seconds = 0;
	timer->milliseconds = 0;
	timer->delta_seconds = 0;
	timer->delta_milliseconds = 0;

	engine->game_loop.cap_framerate = false;
	
	engine->game_loop.target_fps = engine->display.refresh_rate;
	//engine->game_loop.target_fps = 10;
	engine->game_loop.fps = 0;
	engine->game_loop.target_delta_time = 1.0f / engine->game_loop.target_fps;
	//engine->game_loop.delta_time = 0;
    
	// NOTE: the time step is the physics time step.
	// The time step isnt really tied to the gamer render fps
	// Right now the time step is fixed to the target fps
	//engine->game_loop.time_step = 1.0f / engine->game_loop.target_fps;
	engine->game_loop.time_step = 0.01f;
	//engine->game_loop.current_time = SDL_GetTicks() / 1000.0f;
	engine->game_loop.accumulator = 0;
	engine->game_loop.physics_time = 0;
	engine->game_loop.max_delta = 0.05f;
	engine->game_loop.frame_count = 0;
	return true;
}

static bool init_debug(Engine* engine) {
	return true;
}

static bool init_ecs(Engine* engine) {
	init_entity_manager(&engine->entity_manager);
	return true;
}

static bool init_asset_manager(Engine* engine) {
	LOG_INFO("ENGINE", "Starting up asset manager subsystem");
	init_asset_manager(&engine->asset_manager);
	return true;
}



static void poll_inputs(Engine* engine) {
	post_update_button_state(&engine->input.mouse.mouse_button_left);
	post_update_button_state(&engine->input.mouse.mouse_button_middle);
	post_update_button_state(&engine->input.mouse.mouse_button_right);
    
	for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
		post_update_button_state(&engine->input.keys[c]);
	}
	// Reset scroll
	engine->input.mouse.scroll = Vec2i(0, 0);
    
	// Handle mouse seperatly
	{
		int x, y;
		int gx, gy;

		SDL_GetGlobalMouseState(&gx, &gy);
		SDL_GetMouseState(&x, &y);
		//debug_print("mouse pos %d,%d\tglobal mos pos %d,%d\n", x, y, gx, gy);

		
        
		Event event;
		event.kind = EventKind::Mouse_Move;
		event.event.mouse_move_event.pos = Vec2i(x, y);
		event.event.mouse_move_event.delta_pos  = Vec2i(x - engine->input.mouse.pos.x, y - engine->input.mouse.pos.y);
        
		
		// if last delta == 0 && new delta  == 0, skip. dont send events if the mouse hasnt moved since last time we polled
		if ((engine->input.mouse.delta_pos.x == 0 && engine->input.mouse.delta_pos.y == 0)
			&& (event.event.mouse_move_event.delta_pos.x == 0 && event.event.mouse_move_event.delta_pos.y == 0)) {
			// Do nothing. We could flip the condition, but this reads more clear
		} else {
			push_to_event_queue(engine, event);
		}

		Event event2;
		event2.kind = EventKind::Mouse_Global_Move;
		event2.event.mouse_move_event.global_pos= Vec2i(gx, gy);
		event2.event.mouse_move_event.global_delta_pos = Vec2i(gx - engine->input.mouse.global_pos.x, gy - engine->input.mouse.global_pos.y);


		// if last delta == 0 && new delta  == 0, skip. dont send events if the mouse hasnt moved since last time we polled
		if ((engine->input.mouse.global_delta_pos.x == 0 && engine->input.mouse.global_delta_pos.y == 0)
			&& (event2.event.mouse_move_event.global_delta_pos.x == 0 && event2.event.mouse_move_event.global_delta_pos.y == 0)) {
			// Do nothing. We could flip the condition, but this reads more clear
		} else {
			push_to_event_queue(engine, event2);
		}
		
	}

	
	
	
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event)) {

#if ENGINE_MODE_EDITOR
		ImGui_ImplSDL2_ProcessEvent(&sdl_event);
#endif
        
		switch (sdl_event.type) {
			// This mouse events dont capture when mouse deltas go to zero
			/*case SDL_MOUSEMOTION: {
    Event event;
    event.kind = EventKind::Mouse_Move;
    event.event.mouse_move_event.pos = (Vec2i) { sdl_event.motion.x, sdl_event.motion.y };;
    event.event.mouse_move_event.delta_pos = (Vec2i) { sdl_event.motion.xrel, sdl_event.motion.yrel };
    push_to_event_queue(engine, event);
    break;
   }*/
			case SDL_MOUSEWHEEL: {
				
				Event event;
				event.kind = EventKind::Mouse_Scroll;
				event.event.mouse_scroll_event.x_scroll = sdl_event.wheel.x;
				event.event.mouse_scroll_event.y_scroll = sdl_event.wheel.y;

				debug_print("Scroll %d, %d\n", sdl_event.wheel.x, sdl_event.wheel.y);
				push_to_event_queue(engine, event);
				break;
			}
            
			case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: {
                
				MouseButton button;
				button = MouseButton::None;
                
				if (sdl_event.button.button == SDL_BUTTON_LEFT) {
					//update_button_state(&engine->mouse.mouse_button_left, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton::Left;
				} else if (sdl_event.button.button == SDL_BUTTON_MIDDLE) {
					//update_button_state(&engine->mouse.mouse_button_middle, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton::Middle;
				} else if (sdl_event.button.button == SDL_BUTTON_RIGHT) {
					//update_button_state(&engine->mouse.mouse_button_right, sdl_event.button.state == SDL_PRESSED);
					button = MouseButton::Right;
				}
                
				if (button != MouseButton::None) {
					Event event;
					event.kind = sdl_event.type == SDL_MOUSEBUTTONDOWN ? EventKind::Mouse_Button_Down : EventKind::Mouse_Button_Up;
					event.event.mouse_button_event.button = button;
					event.event.mouse_button_event.pos = Vec2i(sdl_event.button.x, sdl_event.button.y);
					push_to_event_queue(engine, event);
				}
                
				break;
			}
            
			case SDL_KEYDOWN: case SDL_KEYUP: {
				SDL_Scancode scancode = sdl_event.key.keysym.scancode;

				
				if (scancode) {
					Event event;
					event.kind = sdl_event.type == SDL_KEYDOWN ? EventKind::Key_Down : EventKind::Key_Up;
					u8 repeat = sdl_event.key.repeat;

					event.event.key_event.key = scancode;
					event.event.key_event.repeat = repeat > 0;
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
					case SDL_WINDOWEVENT_MOVED: {event.kind = EventKind::Window_Moved;	break; }
					case SDL_WINDOWEVENT_RESIZED: {event.kind = EventKind::Window_Resized;	break; }
					case SDL_WINDOWEVENT_MINIMIZED: {event.kind = EventKind::Window_Minimized;	break; }
					case SDL_WINDOWEVENT_MAXIMIZED: {event.kind = EventKind::Window_Maximized;	break; }
					case SDL_WINDOWEVENT_RESTORED: {event.kind = EventKind::Window_Restored;	break; }
					case SDL_WINDOWEVENT_SHOWN: {event.kind = EventKind::Window_Shown;	break; }
					case SDL_WINDOWEVENT_HIDDEN: {event.kind = EventKind::Window_Hidden;	break; }
					case SDL_WINDOWEVENT_ENTER: {event.kind = EventKind::Window_Enter_Mouse_Focus;	break; }
					case SDL_WINDOWEVENT_LEAVE: {event.kind = EventKind::Window_Lose_Mouse_Focus;	break; }
					case SDL_WINDOWEVENT_CLOSE: {event.kind = EventKind::Window_Closed;	break; }
					case SDL_WINDOWEVENT_FOCUS_GAINED: {event.kind = EventKind::Window_Enter_Keyboard_Focus;	break; }
					case SDL_WINDOWEVENT_FOCUS_LOST: {event.kind = EventKind::Window_Lose_Keyboard_Focus;	break; }
					default: {
						event.kind = EventKind::None;
						break;
					}
                    
				}
				push_to_event_queue(engine, event);
                
                
				break;
			}
            
			case SDL_QUIT: {
				engine->quit = true;
				debug_print("Engine quit\n");
				break;
			}


			// Handle windows specifc events
			case SDL_SYSWMEVENT: {
				SDL_SysWMmsg* sysmsg = sdl_event.syswm.msg;
				switch (sysmsg->subsystem) {
					
					// Handle windows specific events
					case SDL_SYSWM_WINDOWS: {
						switch (sysmsg->msg.win.msg) {
							case WM_CLOSE: {
								debug_print("win_close \n");
								engine->quit = true;
								break;
							}
							case WM_QUIT: {
								debug_print("win_quit \n");
								engine->quit = true;
								break;
							}
							case WM_DESTROY: {
								debug_print("win_destroy\n");
								engine->quit = true;
								break;
							}
							
						}
					
						
						

						break;
					}
				}
				
				break;
			}
            
		}
	}
}

static bool load_engine_into_mode(Engine* engine) {
    
    
	EngineAPI api = {
		&engine->display,
		&engine->window,
		&engine->input,
		&engine->game_loop,
		&engine->entity_manager,
		&engine->renderer,
		&engine->asset_manager,
	};


	attach_engine_subsytems(&engine->loaded_game, api);

#if ENGINE_MODE_EDITOR

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	
	
	const char* glsl_version = "#version 330 core";
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(engine->window.sdl_window, engine->renderer.opengl.gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	LOG_INFO("ENGINE", "Loading Engine Editor Interface");
	if (!init_editor_interface(&engine->editor, api)) { return false; }
#else
	LOG_INFO("ENGINE", "Loading Game");
	on_game_start(&engine->loaded_game);
#endif


    
	return true;
}


bool init_engine(Engine* engine) {
    
	

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		LOG_FATAL("ENGINE", "Could not init SDL: %s", SDL_GetError());
		exit(-1);
	}
    
	if (!init_engine_memory(engine)) {
		LOG_FATAL("ENGINE", "Engine failed to allocate memory at startup");
		return false;
	}
    
    
#if DEBUG
	if (!init_debug(engine)) { return false; }
#endif
    
	engine->quit = false;
	engine->platform = SDL_GetPlatform();
	if (!init_display(engine)) { return false; }
	if (!init_window(engine)) { return false; }
	if (!init_keys(engine)) { return false; }
	if (!init_event_queue(engine)) { return false; }
	if (!init_game_loop(engine)) { return false; }
	if (!init_ecs(engine)) { return false; }
	if (!init_asset_manager(engine)) { return false; }
	if (!init_backend_renderer(engine)) { return false; }

	if (!load_engine_into_mode(engine)) { return false; }
	
    
    
    
	return true;
}

bool destroy_engine(Engine* engine) {


	LOG_INFO("ENGINE", "Destroying engine subsystems");


#if ENGINE_MODE_EDITOR

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	destroy_editor_interface(&engine->editor);
#else
	on_game_quit(&engine->loaded_game);
#endif

	
	
	destroy_entity_manager(&engine->entity_manager);
	destroy_asset_manager(&engine->asset_manager);
	destory_backend_renderer(&engine->renderer);

	SDL_DestroyWindow(engine->window.sdl_window);
	SDL_Quit();
    
	
	return true;
}


static void update_engine_state(Engine* engine, float delta_time) {
	
	// Engine specific updates
	if (is_key_pressed(&engine->input, KEYCODE_G)){
		engine->renderer.opengl.show_debug_grid = !engine->renderer.opengl.show_debug_grid;
		engine->renderer.opengl.show_debug_axes = !engine->renderer.opengl.show_debug_axes;
	}
    
	if (is_key_pressed(&engine->input, KEYCODE_L)) {
		engine->renderer.opengl.draw_lines = !engine->renderer.opengl.draw_lines;
	}
    
#if ENGINE_MODE_EDITOR


	// Editor upate
	//editor_update(&engine->editor);
	if (is_key_pressed(&engine->input, KEYCODE_ESCAPE)) {
		engine->quit = true;
	}

#else
	game_update(&engine->loaded_game);
	
	// Game update
	if (is_key_pressed(&engine->input, KEYCODE_ESCAPE)) {
		engine->quit = true;
	}
#endif
    
    
    
	
	

    
	EntityManager* entity_manager = &engine->entity_manager;
	Renderer* renderer = &engine->renderer;
	AssetManager* asset_manager = &engine->asset_manager;
	Vec2i window_size = engine->window.size;


	
    
    
	editor_update(&engine->editor);

	job_update_basis_vectors(entity_manager);
	job_compute_world_matrices(entity_manager);
	job_compute_camera_view_matrices(entity_manager);

	editor_post_update(&engine->editor);

	
    
    
	// You go through all the entites, and push them to the render state
	for (int i = 0; i < entity_manager->camera_manager.enabled_count; i++) {
		Camera* cam = &entity_manager->camera_manager.enabled_cameras[i];
		Entity e = entity_manager->camera_manager.enabled_cameras[i].entity_ref;
		Vec3f cam_pos = get_position(entity_manager, e);
		push_camera(renderer, cam, cam_pos);
	}
    
    
	for (int i = 0; i < entity_manager->light_manager.enabled_count; i++) {
		Light light = entity_manager->light_manager.lights[i];
		push_light(renderer, light);
	}
    

	
    
    
	for (int i = 0; i < entity_manager->render_manager.enabled_count; i++) {
        
		Entity e = entity_manager->render_manager.renders[i].entity_ref;
		
		
		
		StaticMeshID mesh_id = get_static_mesh(entity_manager, e);
		// Might be bad for cache misses?
		// Need to investigate
		// If when removing mesh components, should we also remove the render component?
		if (mesh_id.id == 0) {
			// No Mesh to render
			continue;
		}
		
		MaterialID mat_id = get_render_material(entity_manager, e);

		if (mat_id.id == 0) {
			continue;
		}

		RenderMesh rm;
		rm.mesh = get_static_mesh_by_id(asset_manager, mesh_id);
		rm.world = get_world_mat(entity_manager, e);

		//how to create material and also handling asset / material streaming
		//do we do an if here and see if it is loaded and load here ?

		rm.material = get_material_internal_by_id(asset_manager, mat_id);

		if (!is_material_loaded(renderer, mat_id)) {
			create_material_resource(renderer, rm.material);
		}

		
		push_render_object(renderer, rm);
	}
    
    
	
}

static void fixed_physics_update(Engine* engine, float fixed_time) {
	// Physics stuff
}

void game_loop(Engine* engine) {




	while (!engine->quit) {

		GameTimer* timer = &engine->game_loop;
		u64 ticks = SDL_GetPerformanceCounter() - timer->sdl_start_ticks;
		timer->delta_ticks = (int)(ticks - timer->ticks);
		timer->ticks = ticks;
		

		timer->milliseconds = (timer->ticks * 1000) / timer->ticks_per_sec;
		timer->seconds = (double)(timer->ticks) / (double)(timer->ticks_per_sec);

		timer->delta_milliseconds = (int)((timer->delta_ticks * 1000) / timer->ticks_per_sec);
		timer->delta_seconds = (float)(timer->delta_ticks) / (float)(timer->ticks_per_sec);
		
		
		timer->delta_time = timer->delta_milliseconds / 1000.0f;
		
		
		timer->frame_count++;
		timer->fps = (int)(1 / timer->delta_time);


		timer->target_delta_time = 1.0f / timer->target_fps;
		timer->accumulator += timer->delta_time;


		
        
		// Process inputs once per frame
		poll_inputs(engine);
		process_event_queue(engine);
        
		while (timer->accumulator >= timer->time_step) {
			// fixed update zero or more times per frame
			fixed_physics_update(engine, timer->time_step);
			timer->physics_time += timer->time_step;
			timer->accumulator -= timer->time_step;
		}

		// Update once per frame
		update_engine_state(engine, timer->delta_time);
		
        
		// TODO: this is used to interpolate game state during the rendering
		float alpha = timer->accumulator / timer->time_step;
        
        
        
		
		

		switch (engine->renderer.type) {
			case BackenedRendererType::Software: {
				software_render(&engine->renderer.software_renderer);
				software_debug_render(&engine->renderer.software_renderer);
				software_swap_buffer(&engine->renderer.software_renderer);
				break;
			}
            
			case BackenedRendererType::OpenGL: {
				opengl_render(&engine->renderer.opengl, true);
				opengl_swap_buffer(&engine->renderer.opengl);
				break;
			}
			default:
            break;
		}

		clear_render_world(&engine->renderer);
        
        
	
        
		if (timer->cap_framerate) {
			// TODO: if we have low frame rate, we might be skipping events. A possible  way to do this, is to have the input and rendering on seperate threads
			// https://gamedev.stackexchange.com/questions/90762/taking-advantage-of-multithreading-between-game-loop-and-opengl

			u64 frame_ticks = (SDL_GetPerformanceCounter() - timer->sdl_start_ticks) - timer->ticks;
			float frame_time = ((1.0f * frame_ticks) / (1.0f * timer->ticks_per_sec));
			
			if (frame_time < timer->target_delta_time) {
				float sleep_for = timer->target_delta_time - frame_time;
				// NOTE: since we can't change the os scheduling, we'll wait at least sleep_for time.
				// If there is a way to change the granularity of the sleep, then we should do it
				SDL_Delay((Uint32)(1000 * sleep_for));
			}
		}
		
        
	}
}


void editor_loop(Engine* engine) {
	while (!engine->quit) {
		// Start the Dear ImGui frame
		
		
		


		GameTimer* timer = &engine->game_loop;
		u64 ticks = SDL_GetPerformanceCounter() - timer->sdl_start_ticks;
		timer->delta_ticks = (int)(ticks - timer->ticks);
		timer->ticks = ticks;
		timer->milliseconds = (timer->ticks * 1000) / timer->ticks_per_sec;
		timer->seconds = (double)(timer->ticks) / (double)(timer->ticks_per_sec);
		timer->delta_milliseconds = (int)((timer->delta_ticks * 1000) / timer->ticks_per_sec);
		timer->delta_seconds = (float)(timer->delta_ticks) / (float)(timer->ticks_per_sec);
		timer->delta_time = timer->delta_milliseconds / 1000.0f;
		timer->frame_count++;
		timer->fps = (int)(1 / timer->delta_time);
		timer->target_delta_time = 1.0f / timer->target_fps;
		timer->accumulator += timer->delta_time;




		

		// Process inputs once per frame
		poll_inputs(engine);
		process_event_queue(engine);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(engine->window.sdl_window);
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		
		

		


		while (timer->accumulator >= timer->time_step) {
			// fixed update zero or more times per frame
			fixed_physics_update(engine, timer->time_step);
			timer->physics_time += timer->time_step;
			timer->accumulator -= timer->time_step;
		}

		// Update once per frame
		update_engine_state(engine, timer->delta_time);


		// TODO: this is used to interpolate game state during the rendering
		float alpha = timer->accumulator / timer->time_step;

		switch (engine->renderer.type) {
			case BackenedRendererType::OpenGL: {
				ImGui::Render();
				
				opengl_render(&engine->renderer.opengl, true);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				opengl_swap_buffer(&engine->renderer.opengl);
				break;
			}
			default:
				break;
		}

		clear_render_world(&engine->renderer);



		if (timer->cap_framerate) {
			// TODO: if we have low frame rate, we might be skipping events. A possible  way to do this, is to have the input and rendering on seperate threads
			// https://gamedev.stackexchange.com/questions/90762/taking-advantage-of-multithreading-between-game-loop-and-opengl

			u64 frame_ticks = (SDL_GetPerformanceCounter() - timer->sdl_start_ticks) - timer->ticks;
			float frame_time = ((1.0f * frame_ticks) / (1.0f * timer->ticks_per_sec));

			if (frame_time < timer->target_delta_time) {
				float sleep_for = timer->target_delta_time - frame_time;
				// NOTE: since we can't change the os scheduling, we'll wait at least sleep_for time.
				// If there is a way to change the granularity of the sleep, then we should do it
				SDL_Delay((Uint32)(1000 * sleep_for));
			}
		}
	}
}
