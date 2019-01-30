#pragma once

#include "Core/ECS/Component/RenderComponent.h"
#include "Common/stretchy_buffer.h"

void init_render_manager(RenderManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->renders = NULL;

}
void destroy_render_manager(RenderManager* manager) {
	stb_sb_free(manager->renders);
	map_destroy(&manager->id_map);
}

void entity_add_render_component(RenderManager* manager, Entity entity) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->renders, Render(entity, true));
}

void entity_remove_render_component(RenderManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;
	// Get the last in the list to swap with
	Render last = manager->renders[manager->count - 1];
	// swap the last at the current index we are removing from
	manager->renders[index] = last;
	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);



	
}

