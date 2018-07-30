#pragma once

#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Renderer/SoftwareRenderer.h"

#define DEBUG 1

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
	WindowFlag_Fullscreen = 1 << 0,
	WindowFlag_Hidden = 1 << 3,
	WindowFlag_Resizable = 1 << 5,
	WindowFlag_Minimized = 1 << 6,
	WindowFlag_Maximized = 1 << 7,
	WindowFlag_Has_Keyboard_Focus = 1 << 9,
	WindowFlag_Has_Mouse_Focus = 1 << 10,

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
	SoftwareRenderer graphics;
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



static void update_button_state(ButtonState* button_state, bool down);
static void reset_button_state(ButtonState* button_state);
static void log_error(const char* name);
static bool push_to_event_queue(App* app, Event event);
static void process_event_queue(App* app);
static bool init_display(App* app);
static bool init_window(App* app);
static bool init_keys(App* app);
static bool init_event_queue(App* app);
static bool init_time(App* app);
static bool init_debug(App* app);
static void update_time(App* app);
static void process_inputs(App* app);

bool init_app(App* app);
bool destroy_app(App* app);
void game_loop(App* app);
