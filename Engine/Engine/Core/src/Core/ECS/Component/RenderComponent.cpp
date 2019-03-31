#pragma once

#include "Core/ECS/Component/RenderComponent.h"
#include "Common/stretchy_buffer.h"

void init_render_manager(RenderManager* manager) {
	map_init(&manager->id_map);
	
	manager->capacity = 0;
	manager->enabled_count = 0;
	manager->renders = NULL;
	manager->entitys = NULL;

}

void destroy_render_manager(RenderManager* manager) {
	stb_sb_free(manager->renders);
	stb_sb_free(manager->entitys);
	map_destroy(&manager->id_map);
	manager->capacity = 0;
	manager->enabled_count = 0;
}

bool entity_add_render_component(RenderManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There already a component, return early and do nothing
	if (result.found) return false;

	
	
	u64 index;

	if (manager->enabled_count == manager->capacity && stb_sb_count(manager->renders) != manager->enabled_count) {
		manager->renders[manager->enabled_count] = Render(entity);
		manager->entitys[manager->enabled_count] = entity;
		index = manager->enabled_count;
	} else {
		stb_sb_push(manager->renders, Render(entity));
		stb_sb_push(manager->entitys, entity);
		index = stb_sb_count(manager->renders) - 1;
	}

	

	


	Render this_comp = manager->renders[index];
	Render first_disabled_comp = manager->renders[manager->enabled_count];


	// swap the first disabled comp with the comp we are trying to enable
	manager->renders[manager->enabled_count] = this_comp;
	manager->renders[index] = first_disabled_comp;

	// Swap entitys
	Entity this_entity = manager->entitys[index];
	Entity first_disabled_entity = manager->entitys[manager->enabled_count];
	manager->entitys[manager->enabled_count] = this_entity;
	manager->entitys[index] = first_disabled_entity;

	// Update the entity id to index mapping
	map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	map_put(&manager->id_map, first_disabled_entity.id, index);


	manager->enabled_count++;
	manager->capacity++;

	assert(manager->enabled_count <= manager->capacity);

	return true;

}

bool entity_remove_render_component(RenderManager* manager, Entity entity) {

	

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return false;

	//removing disabled components
	u64 index = result.value;

	if (index >= manager->enabled_count) {
		Render this_comp = manager->renders[index];
		Render last_comp = manager->renders[manager->capacity - 1];
	
		// swap the last at the current index we are removing from
		manager->renders[index] = last_comp;
	
		// Swap entitys
		Entity last_entity = manager->entitys[manager->capacity - 1];
		manager->entitys[index] = last_entity;
	
		// Remove the entity from the index map
		map_remove(&manager->id_map, entity.id);
		if (entity.id != last_entity.id) {
			map_put(&manager->id_map, last_entity.id, index);
		}
	
		manager->capacity--;
		assert(manager->enabled_count <= manager->capacity);
		return true;
	}
	
	 
	
	// Get the last in the list to swap with

	Render this_comp = manager->renders[index];
	Render last_comp = manager->renders[manager->enabled_count - 1];

	// swap the last at the current index we are removing from
	manager->renders[index] = last_comp;

	// Swap entitys
	Entity last_entity = manager->entitys[manager->enabled_count - 1];
	manager->entitys[index] = last_entity;

	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
	if (entity.id != last_entity.id) {
		map_put(&manager->id_map, last_entity.id, index);
	}
	manager->enabled_count--;
	manager->capacity--;
	assert(manager->enabled_count <= manager->capacity);
	return true;
}



void enable_render_component(RenderManager* manager, Entity entity, bool enabled) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;

	if (enabled) {
		

		Render this_comp = manager->renders[index];
		Render first_disabled_comp = manager->renders[manager->enabled_count];
		

		// swap the first disabled comp with the comp we are trying to enable
		manager->renders[manager->enabled_count] = this_comp;
		manager->renders[index] = first_disabled_comp;

		// Swap entitys
		Entity this_entity = manager->entitys[index];
		Entity first_disabled_entity = manager->entitys[manager->enabled_count];
		manager->entitys[manager->enabled_count] = this_entity;
		manager->entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(&manager->id_map, this_entity.id, manager->enabled_count);
		map_put(&manager->id_map, first_disabled_entity.id, index);

		manager->enabled_count++;
	} else {

		
		Render this_comp = manager->renders[index];
		Render last_comp = manager->renders[manager->enabled_count - 1];
		// swap the last at the current index we are removing from
 		manager->renders[index] = last_comp;
		manager->renders[manager->enabled_count - 1] = this_comp;
		
		// Swap entitys
		Entity this_entity = manager->entitys[index];
		Entity first_disabled_entity = manager->entitys[manager->enabled_count - 1];
		manager->entitys[manager->enabled_count - 1] = this_entity;
		manager->entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(&manager->id_map, this_entity.id, manager->enabled_count - 1);
		map_put(&manager->id_map, first_disabled_entity.id, index);

		manager->enabled_count--;
	}
	assert(manager->enabled_count <= manager->capacity);
	
}

bool is_render_component_enabled(RenderManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);

	if (!result.found) {
		return false;
	}
	// Check if the index is less than the enabled index
	// Everything to the left of the enabeld count is enabled, everything to the right of it, is disabled
	assert(manager->enabled_count <= manager->capacity);
	return result.value < manager->enabled_count;
	
	
}