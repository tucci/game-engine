#pragma once

#include "Core/ECS/Component/ComponentHelpers.h"
#include "Core/ECS/Component/Lights.h"
#include "Common/stretchy_buffer.h"

void init_light_manager(LightManager* manager) {
	map_init(&manager->id_map);

	manager->total_count = 0;
	manager->enabled_count = 0;
	manager->lights = NULL;
	manager->entitys = NULL;

	

	
}

void destroy_light_manager(LightManager* manager) {
	stb_sb_free(manager->lights);
	stb_sb_free(manager->entitys);
	map_destroy(&manager->id_map);
	manager->total_count = 0;
	manager->enabled_count = 0;

	


}



bool entity_add_light_component(LightManager* manager, Entity entity) {

	

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There already a component, return early and do nothing
	if (result.found) return false;


	u64 index;
	u64 count = stb_sb_count(manager->entitys);
	if (manager->enabled_count == manager->total_count && count != manager->enabled_count) {
		manager->entitys[manager->enabled_count] = entity;
		index = manager->enabled_count;
	} else {
		stb_sb_push(manager->entitys, entity);
		index = stb_sb_count(manager->entitys) - 1;
	}





	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->lights, Light(), index);

	// Swap entitys
	Entity this_entity = manager->entitys[index];
	Entity first_disabled_entity = manager->entitys[manager->enabled_count];
	manager->entitys[manager->enabled_count] = this_entity;
	manager->entitys[index] = first_disabled_entity;

	// Update the entity id to index mapping
	map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	map_put(&manager->id_map, first_disabled_entity.id, index);

	manager->enabled_count++;
	manager->total_count++;

	assert(manager->enabled_count <= manager->total_count);

	return true;
}

bool entity_remove_light_component(LightManager* manager, Entity entity) {
	

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return false;


	u64 index = result.value;
	u64 index_to_swap;

	//removing disabled components
	if (index >= manager->enabled_count) {
		index_to_swap = manager->total_count - 1;
	} else {
		index_to_swap = manager->enabled_count - 1;
	}


	comphelper_remove_component_data(entity, manager->lights, index, index_to_swap);

	Entity last_entity = manager->entitys[index_to_swap];
	manager->entitys[index] = last_entity;

	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
	if (entity.id != last_entity.id) {
		map_put(&manager->id_map, last_entity.id, index);
	}

	manager->enabled_count--;
	manager->total_count--;
	assert(manager->enabled_count <= manager->total_count);

	return true;
}

void enable_light_component(LightManager* manager, Entity entity, bool enabled) {
	
	comphelper_enable_component(&manager->id_map, manager->entitys, &manager->enabled_count, manager->lights, entity, enabled);
	assert(manager->enabled_count <= manager->total_count);
}

bool is_light_component_enabled(LightManager* manager, Entity entity) {
	// Check if the index is less than the enabled index
	// Everything to the left of the enabeld count is enabled, everything to the right of it, is disabled
	assert(manager->enabled_count <= manager->total_count);

	return comphelper_is_component_enabled(&manager->id_map, &manager->enabled_count, entity);
}