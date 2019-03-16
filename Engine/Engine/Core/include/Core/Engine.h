#pragma once

#include <SDL.h>
#include <SDL_syswm.h>

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

#include "Editor/EditorInterface.h"

#define ENGINE_MODE_EDITOR 1





// TODO: move these to a config file
#define WINDOW_TITLE "Engine"
#define DEFAULT_WINDOW_SIZE_X 1024
#define DEFAULT_WINDOW_SIZE_Y 720
#define WINDOW_RESIZEABlE true
#define MAX_EVENTS 32



enum class EventKind {
	None,
	Key_Down,
	Key_Up,
	Mouse_Button_Down,
	Mouse_Button_Up,
	Mouse_Move,
	Mouse_Global_Move,
	Mouse_Scroll,
	Window_Moved,
	Window_Resized,
	Window_Minimized,
	Window_Maximized,
	Window_Restored,
	Window_Shown,
	Window_Hidden,
	Window_Closed,
	Window_Enter_Mouse_Focus,
	Window_Lose_Mouse_Focus,
	Window_Enter_Keyboard_Focus,
	Window_Lose_Keyboard_Focus,
};

struct KeyEvent {
	int key;
};

struct MouseButtonEvent {
	MouseButton button;
	Vec2i pos;
};

struct MouseScrollEvent {
	s32 x_scroll;
	s32 y_scroll;
};

struct MouseMoveEvent {
	Vec2i pos;
	Vec2i delta_pos;

	Vec2i global_pos;
	Vec2i global_delta_pos;
};

enum class WindowEventKind {
	WindowEventKind_Moved
};

struct WindowEventData {
	u32 window_id;
	int data1;
	int data2;

};

struct WindowEvent {
	WindowEventKind kind;
	WindowEventData data;
};

union EventData {
	KeyEvent key_event;
	MouseButtonEvent mouse_button_event;
	MouseMoveEvent mouse_move_event;
	MouseScrollEvent mouse_scroll_event;
	WindowEvent window_event;
	EventData() {};
};

struct Event {
	EventKind kind;
	EventData event;
	
};





struct Engine {

	// Exposed subsystems that the game can query, though should not modify
	Display display;
	Window window;
	Input input;
	GameTimer game_loop;
	EntityManager entity_manager;
	AssetManager asset_manager;
	Renderer renderer;
	EditorInterface editor;

	
	


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
};




static bool push_to_event_queue(Engine* engine, Event event);
static void process_event_queue(Engine* engine);

static bool init_engine_memory(Engine* engine);
static bool init_display(Engine* engine);
static bool init_window(Engine* engine);
static bool init_backend_renderer(Engine* engine);
static bool init_keys(Engine* engine);
static bool init_event_queue(Engine* engine);
static bool init_game_loop(Engine* engine);
static bool init_debug(Engine* engine);
static bool init_ecs(Engine* engine);
static bool init_asset_manager(Engine* engine);

static void update_clock(Engine* engine);
static void poll_inputs(Engine* engine);

static bool load_engine_into_mode(Engine* engine);


bool init_engine(Engine* engine);
bool destroy_engine(Engine* engine);
static void update_engine_state(Engine* engine, float delta_time);
static void fixed_physics_update(Engine* engine, float fixed_time);
void game_loop(Engine* engine);

