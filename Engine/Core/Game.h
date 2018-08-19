#pragma once

#include <stdbool.h>
#include "Scene.h"

#include "../Common/LinearAllocator.h"



typedef struct Game {
	LinearAllocator game_memory;
	Scene* loaded_scene;
} Game;



void load_scene(Game* game, int scene_id);
void unload_scene(Game* game, Scene* scene);