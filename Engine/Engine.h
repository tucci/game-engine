#pragma once

#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Core/Game.h"
#include "Core/Input.h"
#include "Core/GameTimer.h"


#include "Renderer/software_renderer/SoftwareRenderer.h"
#include "Renderer/gl/OpenGLRenderer.h"

#include "debug_macros.h"


#define WINDOW_TITLE "Engine"
#define DEFAULT_WINDOW_SIZE_X 800
#define DEFAULT_WINDOW_SIZE_Y 800
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
	WindowFlag_OpenGL = 1 << 1,
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

typedef struct Clock {

	uint64_t ticks;
	uint64_t delta_ticks;

	double seconds;
	float delta_seconds;

	uint64_t milliseconds;
	int delta_milliseconds;
	

	uint64_t ticks_per_sec;
	uint64_t sdl_start_ticks;

} Clock;



typedef enum BackenedRendererType {
	BackenedRenderer_Software,
	BackenedRenderer_OpenGL
} BackenedRendererType;



typedef struct Renderer {
	BackenedRendererType type;
	union {
		SoftwareRenderer software_renderer;
		OpenGLRenderer opengl;
	};
	
} Renderer;


typedef struct MemoryEnginePartition {
	void* start_ptr;
	size_t partition_size;
} MemoryEnginePartition;






typedef struct Engine {


	Renderer renderer;
	Display display;
	Window window;

	Input input;

	Event event_queue[MAX_EVENTS];
	int event_count;

	Clock clock;
	GameTimer game_loop;

	char const* platform;
	bool quit;



	// Fixed memory for the entire engine/game
	void* engine_memory;
	size_t engine_memory_size;
	void* partition_ptr;

	// TODO: find a better way to hold this reference
	Game* loaded_game;
	
	

	
} Engine;



static void log_error(const char* name);
static bool push_to_event_queue(Engine* engine, Event event);
static void process_event_queue(Engine* engine);

static bool init_engine_memory(Engine* engine);
MemoryEnginePartition give_memory_partition(Engine* engine, size_t size);

static bool init_display(Engine* engine);
static bool init_window(Engine* engine);
static bool init_renderer(Engine* engine);
static bool init_keys(Engine* engine);
static bool init_event_queue(Engine* engine);
static bool init_clock(Engine* engine);
static bool init_game_loop(Engine* engine);
static bool init_debug(Engine* engine);
static void update_clock(Engine* engine);

static void process_inputs(Engine* engine);

static bool load_game(Engine* engine, const char* game_file);


bool init_engine(Engine* engine);
bool destroy_engine(Engine* engine);
static void update(Engine* engine, float delta_time);
static void fixed_update(Engine* engine, float fixed_time);
void game_loop(Engine* engine);

