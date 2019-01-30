#pragma once

#include "Core/ECS/Component/Lights.h"
#include "Common/stretchy_buffer.h"

void init_light_manager(LightManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->lights = NULL;
}

void destroy_light_manager(LightManager* manager) {
	stb_sb_free(manager->lights);
	map_destroy(&manager->id_map);
}
void entity_add_light_component(LightManager* manager, Entity entity) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->lights, Light());
}

void entity_remove_light_component(LightManager* manager, Entity entity) {
	
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;
	// Get the last in the list to swap with
	Light last = manager->lights[manager->count - 1];
	// swap the last at the current index we are removing from
	manager->lights[index] = last;
	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
}