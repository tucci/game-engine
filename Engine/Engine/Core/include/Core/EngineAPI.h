#pragma once


#include "Core/Display.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/GameTimer.h"
#include "Core/ECS/EntityManager.h"
#include "Core/Renderer/Renderer.h"
#include "Asset/AssetManager.h"

//struct Display;
//struct Window;
//struct Input;
//struct GameTimer;
//struct EntityManager;
//struct Renderer;
//struct AssetManager;



// The API we expose to the underlying game
// NOTE: the api should be all ptrs to the underlying engine
// Since we create a new struct everytime we load the game, we are sending a copy of this struct down
// Therefore this struct should be lightweight
struct EngineAPI {
	Display* display;
	Window* window;
	Input* input;
	GameTimer* game_loop;
	EntityManager* entity_manager;
	Renderer* renderer;
	AssetManager* asset_manager;
};

