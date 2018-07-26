#pragma once

#include <SDL.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "Renderer.h"





#define DEBUG 0

#if DEBUG
	#define DEBUG_ALLOW_PRINT_EVENT true
	#define event_printf(...) printf(__VA_ARGS__)
	
#else
	#define DEBUG_ALLOW_PRINT_EVENT false
	#define NDEBUG 
	#define event_printf(...) do {} while (0)
#endif

#include <assert.h>



// TODO create a proper window title/size default
#define WINDOW_TITLE "Test window"
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 800
#define WINDOW_RESIZEABlE true
#define MAX_EVENTS 32



typedef struct Display {
    float dpi;
    int refresh_rate;
    int w;
    int h;
    
} Display;

typedef enum WindowFlag {
	// NOTE:(steven) These flags try to have the same values as the flags from SDL_WindowFlags
	WindowFlag_None = 0,
	WindowFlag_Fullscreen = 1<<0,
	WindowFlag_Hidden = 1<<3,
	WindowFlag_Resizable = 1<<5,
	WindowFlag_Minimized = 1<<6,
	WindowFlag_Maximized = 1<<7,
	WindowFlag_Has_Keyboard_Focus = 1<<9,
	WindowFlag_Has_Mouse_Focus = 1<<10,

} WindowFlag;

typedef struct Window {
	uint32_t window_id;

    const char* title;
    Vec2i pos;
    Vec2i size;
	uint32_t flags;

    SDL_Window* sdl_window;
    SDL_WindowFlags sdl_window_flags;
    
} Window;


typedef enum MouseButton {
    MouseButton_None,
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
    
} MouseButton;

typedef struct ButtonState {
    bool down;
    int pressed;
    int released;
} ButtonState;



void update_button_state(ButtonState* button_state, bool down) {
    bool was_down = button_state->down;
    button_state->down = down;
    button_state->pressed = down && !was_down;
    button_state->released = !down && was_down;
}

void reset_button_state(ButtonState* button_state) {
    button_state->down = 0;
    button_state->pressed = 0;
    button_state->released = 0;
}

typedef struct Mouse {
    Vec2i pos;
    Vec2i delta_pos;
    
    ButtonState mouse_button_left;
    ButtonState mouse_button_middle;
    ButtonState mouse_button_right;
    
} Mouse;


typedef enum EventKind {
    EventKind_None,
    EventKind_Key_Down,
	EventKind_Key_Up,
	EventKind_Mouse_Button_Down,
	EventKind_Mouse_Button_Up,
	EventKind_Mouse_Move,
	EventKind_Window_Moved,
	EventKind_Window_Resized,
	EventKind_Window_Minimized,
	EventKind_Window_Maximized,
	EventKind_Window_Restored,
	EventKind_Window_Shown,
	EventKind_Window_Hidden,
	EventKind_Window_Enter_Mouse_Focus,
	EventKind_Window_Lose_Mouse_Focus,
	EventKind_Window_Enter_Keyboard_Focus,
	EventKind_Window_Lose_Keyboard_Focus,
} EventKind;

typedef struct KeyEvent {
	int key;
} KeyEvent;

typedef struct MouseButtonEvent {
	MouseButton button;
	Vec2i pos;
} MouseButtonEvent;

typedef struct MouseMoveEvent {
	Vec2i pos;
	Vec2i delta_pos;
} MouseMoveEvent;

typedef enum WindowEventKind {
	WindowEventKind_Moved
} WindowEventKind;

typedef struct WindowEventData {
	uint32_t window_id;
	int data1;
	int data2;

} WindowEventData;

typedef struct WindowEvent {
	WindowEventKind kind;
	WindowEventData data;
} WindowEvent;

typedef union EventData {
	KeyEvent key_event;
	MouseButtonEvent mouse_button_event;
	MouseMoveEvent mouse_move_event;
	WindowEvent window_event;
} EventData;

typedef struct Event {
    EventKind kind;
	EventData event;
    
} Event;

typedef struct Time {

	uint64_t ticks;
	uint64_t delta_ticks;

	double seconds;
	float delta_seconds;

	uint64_t milliseconds;
	int delta_milliseconds;

	uint64_t ticks_per_sec;
	uint64_t sdl_start_ticks;
} Time;


typedef struct DebugData {
	bool print_events;
} DebugData;



typedef struct App {
	//GraphicsRenderer graphics;
	Renderer graphics;
    Display display;
    Window window;
    Mouse mouse;
    ButtonState keys[SDL_NUM_SCANCODES];
    Event event_queue[MAX_EVENTS];
	int event_count;
	Time time;
	char const* platform;
    bool quit;
	DebugData debug;
	

} App;



// TODO: create a error buffer, with a dump to file 
void log_error(const char* name) {
    const char* error = SDL_GetError();
    if (*error) {
        printf("Name %s, SDl_Error %s", name, error);
    }
    
}

bool push_to_event_queue(App* app, Event event) {
	
	if (app->event_count > MAX_EVENTS - 1) {
		printf("Event queue full");
		return false;
	}
	app->event_queue[app->event_count] = event;
	app->event_count++;
	return true;
}

void process_event_queue(App* app) {
	for (int i = 0; i < app->event_count; i++) {
		Event event = app->event_queue[i];
		switch (event.kind) {

			case EventKind_Key_Down: {
				switch (event.event.key_event.key) {
					case SDL_SCANCODE_W: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Up);
						break;
					}
					case SDL_SCANCODE_S: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Down);
						break;
					}

					case SDL_SCANCODE_UP: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Forward);
						break;
					}
					case SDL_SCANCODE_DOWN: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Backward);
						break;
					}

					case SDL_SCANCODE_E: {
						app->graphics.camera.rotation.y += 1;
						break;
					}
					case SDL_SCANCODE_Q: {
						app->graphics.camera.rotation.y -= 1;
						break;
					}

					case SDL_SCANCODE_RIGHT: {
						app->graphics.camera.rotation.z += 1;
						break;
					}
					case SDL_SCANCODE_LEFT: {
						app->graphics.camera.rotation.z -= 1;
						break;
					}

					case SDL_SCANCODE_A: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Left);
						break;
					}
					case SDL_SCANCODE_D: {
						app->graphics.camera.pos.xyz_ = vec_add(app->graphics.camera.pos.xyz_, Vec3f_Right);
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


bool init_display(App* app) {
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



bool init_window(App* app) {
    
    SDL_WindowFlags sdl_flags = 0;
	app->window.flags = 0;

    app->window.flags |= WindowFlag_Resizable;
	//app->window.flags |= WindowFlag_Fullscreen;
    
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
        x,y,
        w,h,
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
    app->window.pos = (Vec2i){ x, y };
    app->window.size = (Vec2i){ w, h };
    app->window.sdl_window = sdl_window;
    app->window.sdl_window_flags = sdl_flags;

	init_renderer(app->window.sdl_window, &app->graphics, app->window.size);
    
    return true;
}



bool init_keys(App* app) {
    // Init mouse buttons
    reset_button_state(&app->mouse.mouse_button_left);
    reset_button_state(&app->mouse.mouse_button_middle);
    reset_button_state(&app->mouse.mouse_button_right);
    
    for (int c = 0; c < SDL_NUM_SCANCODES; c++) {
        reset_button_state(&app->keys[c]);
    }
    
    return true;
}

bool init_event_queue(App* app) {
	app->event_count = 0;
	return true;
}

bool init_time(App* app) {
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

bool init_debug(App* app) {
	app->debug.print_events = DEBUG_ALLOW_PRINT_EVENT;
	return true;
}

bool init_app(App* app) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Could not init SDL: %s \n", SDL_GetError());
        exit(-1);
    }


	#if DEBUG
	if (!init_debug(app)) {return false;}
	#endif

    app->quit = false;
	app->platform = SDL_GetPlatform();
    if (!init_display(app)) {return false;}
    if (!init_window(app)) {return false;}
    if (!init_keys(app)) {return false;}
	if (!init_event_queue(app)) {return false;}
	if (!init_time(app)) {return false;}

	

	return true;
}

bool destroy_app(App* app) {
	

	destroy_renderer(&app->graphics);
	SDL_DestroyWindow(app->window.sdl_window);
	SDL_Quit();
	return true;
}

void update_time(App* app) {
	uint64_t ticks = SDL_GetPerformanceCounter() - app->time.sdl_start_ticks;
	app->time.delta_ticks = (int)(ticks -  app->time.ticks);
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

void process_inputs(App* app) {

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



void game_loop(App* app) {
	while (!app->quit) {
		// TODO: do we need this?
		SDL_PumpEvents();
		process_inputs(app);
		process_event_queue(app);
		render(&app->graphics);
		//debug_render(&app->graphics);
		update_time(app);

	}
}







int main(int argc, char* argv[]) {

	App app;

	if (init_app(&app)) {
		game_loop(&app);
		destroy_app(&app);
	}

    return 0;
}


