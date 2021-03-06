#pragma once


#include "Core/Scene.h"
#include "Core/EngineAPI.h"



#include "Common/StackAllocator.h"



#define GAME_MEMORY MEGABYTES(200)


struct Game {
	// Reference to underlying engine api subsystems the game might need
	EngineAPI engineAPI;


	Arena arena;
	StackAllocator stack;
	Scene* loaded_scene;
	
};


void attach_engine_subsytems(Game* game, EngineAPI api);
void on_game_start(Game* game);
void on_game_quit(Game* game);
void attach_engine_subsytems(Game* game, EngineAPI api);
void load_scene(Game* game, int scene_id);
void unload_scene(Game* game, Scene* scene);

Entity import_asset_scene_into_scene(Game* game, SceneID id);


void game_update(Game* game);