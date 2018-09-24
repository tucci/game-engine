#pragma once


#include "Core/Scene.h"
#include "Core/EngineAPI.h"



#include "Common/StackAllocator.h"






typedef struct Game {
	// Reference to underlying engine api subsystems the game might need
	EngineAPI engineAPI;


	// TODO: move this to a stack allocator
	StackAllocator game_memory;
	Scene* loaded_scene;
	
} Game;


void attach_engine_subsytems(Game* game, EngineAPI api);
void load_scene(Game* game, int scene_id);
void unload_scene(Game* game, Scene* scene);

void game_update(Game* game);