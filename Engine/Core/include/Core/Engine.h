#pragma once

#include <SDL.h>
#include <stdio.h>
#include "types.h"
#include <stdbool.h>

#include "Core/Game.h"
#include "Core/Display.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/GameTimer.h"
#include "Core/Renderer/Renderer.h"
#include "Asset/AssetManager.h"

#include "Core/ECS/EntityManager.h"





#include "debug_macros.h"


// TODO: move these to a config file
#define WINDOW_TITLE "Engine"
#define DEFAULT_WINDOW_SIZE_X 1024
#define DEFAULT_WINDOW_SIZE_Y 720
#define WINDOW_RESIZEABlE true
#define MAX_EVENTS 32









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
	u32 window_id;
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
	EventData() {};
} EventData;

typedef struct Event {
	EventKind kind;
	EventData event;
	
} Event;

typedef struct Clock {

	u64 ticks;
	u64 delta_ticks;

	double seconds;
	float delta_seconds;

	u64 milliseconds;
	int delta_milliseconds;
	

	u64 ticks_per_sec;
	u64 sdl_start_ticks;

} Clock;








typedef struct Engine {

	// Exposed subsystems that the game can query, though should not modify
	Display display;
	Window window;
	Input input;
	Clock clock;
	GameTimer game_loop;
	EntityManager entity_manager;
	AssetManager asset_manager;
	Renderer renderer;
	
	
	
	


	// Internal subsystems
	
	Event event_queue[MAX_EVENTS];
	int event_count;



	char const* platform;
	bool quit;

	// Fixed memory for the entire engine/game
	//void* engine_memory;
	//size_t engine_memory_size;
	//void* partition_ptr;

	
	Game loaded_game;	
	
	Engine() {};
} Engine;




static bool push_to_event_queue(Engine* engine, Event event);
static void process_event_queue(Engine* engine);

static bool init_engine_memory(Engine* engine);
//MemoryEnginePartition give_memory_partition(Engine* engine, size_t size);

static bool init_display(Engine* engine);
static bool init_window(Engine* engine);
static bool init_backend_renderer(Engine* engine);
static bool init_keys(Engine* engine);
static bool init_event_queue(Engine* engine);
static bool init_clock(Engine* engine);
static bool init_game_loop(Engine* engine);
static bool init_debug(Engine* engine);
static bool init_ecs(Engine* engine);
static bool init_asset_manager(Engine* engine);
static void update_clock(Engine* engine);

static void process_inputs(Engine* engine);

static bool load_game(Engine* engine, const char* game_file);


bool init_engine(Engine* engine);
bool destroy_engine(Engine* engine);
static void update_engine_state(Engine* engine, float delta_time);
static void fixed_physics_update(Engine* engine, float fixed_time);
void game_loop(Engine* engine);

