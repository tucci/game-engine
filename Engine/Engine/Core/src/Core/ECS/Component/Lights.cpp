#pragma once

#include "Core/ECS/Component/Lights.h"
#include "Common/stretchy_buffer.h"

void init_light_manager(LightManager* manager) {
	map_init(&manager->enabled_id_map);
	map_init(&manager->disabled_id_map);

	manager->enabled_count = 0;
	manager->enabled_lights = NULL;

	manager->disabled_count = 0;
	manager->disabled_lights = NULL;

	
}

void destroy_light_manager(LightManager* manager) {
	stb_sb_free(manager->enabled_lights);
	map_destroy(&manager->enabled_id_map);
	manager->enabled_count = 0;

	stb_sb_free(manager->disabled_lights);
	map_destroy(&manager->disabled_id_map);
	manager->disabled_count = 0;
}



bool entity_add_light_component(LightManager* manager, Entity entity) {

	

	MapResult<u64> result = map_get(&manager->enabled_id_map, entity.id);
	if (result.found) {
		// There already a component, return early and do nothing
		return false;
	} else {
		// If the component wasn't found inside the enabled list
		// it might be inside the disabled list
		result = map_get(&manager->disabled_id_map, entity.id);
		if (result.found) {
			// There already a component, return early and do nothing
			return false;
		}


	}

	map_put(&manager->enabled_id_map, entity.id, manager->enabled_count);
	

	if (stb_sb_count(manager->enabled_lights) == manager->enabled_count) {
		stb_sb_push(manager->enabled_lights, Light());
	} else {
		manager->enabled_lights[manager->enabled_count] = Light();
	}

	manager->enabled_count++;

	
	return true;
}

bool entity_remove_light_component(LightManager* manager, Entity entity) {
	
	MapResult<u64> result = map_get(&manager->enabled_id_map, entity.id);

	
	CompactMap<u64>* map = &manager->enabled_id_map;
	Light* list = manager->enabled_lights;
	u64* list_count = &manager->enabled_count;

	if (!result.found) {
		// If the component wasn't found inside the enabled list
		// it might be inside the disabled list
		result = map_get(&manager->disabled_id_map, entity.id);
		map = &manager->disabled_id_map;
		list = manager->disabled_lights;
		list_count = &manager->disabled_count;

		if (!result.found) {
			// the component wasn't found inside the disabled list either
			// this component doesnt exist on this entity
			return false;
		}
	}

	u64 index = result.value;
	// Get the last in the list to swap with
	Light last = list[*list_count - 1];
	// swap the last at the current index we are removing from
	list[index] = last;
	(*list_count)--;
	// Remove the entity from the index map
	map_remove(map, entity.id);
	return true;
}

void enable_light_component(LightManager* manager, Entity entity, bool enabled) {
	
	if (enabled) {
		// First we need to check if the component is already enabled
		MapResult<u64> result = map_get(&manager->enabled_id_map, entity.id);

		
		if (result.found) {
			// The component is already enabled
			// don't do anything
			return;
		} 

		
		// If the component wasn't found inside the enabled list
		// it might be inside the disabled list
		result = map_get(&manager->disabled_id_map, entity.id);
		

		if (!result.found) {
			// the component wasn't found inside the disabled list either
			// this component doesnt exist on this entity
			return;
		}


		// At this point the component is on the disabled list and needs to be enabled

		u64 index = result.value;
		Light light_copy = manager->disabled_lights[index];

		entity_remove_light_component(manager, entity);

		entity_add_light_component(manager, entity);

		result = map_get(&manager->enabled_id_map, entity.id);

		index = result.value;
		Light* light_to_copy_to = &manager->enabled_lights[index];
		*light_to_copy_to = light_copy;

	} else {
		// First we need to check if the component is already disabled
		MapResult<u64> result = map_get(&manager->disabled_id_map, entity.id);


		if (result.found) {
			// The component is already disabled
			// don't do anything
			return;
		}


		// If the component wasn't found inside the disabled list
		// it might be inside the enabled list
		result = map_get(&manager->enabled_id_map, entity.id);


		if (!result.found) {
			// the component wasn't found inside the enabled list either
			// this component doesnt exist on this entity
			return;
		}


		// At this point the component is on the enabled list and needs to be disabled
		u64 index = result.value;
		Light light_copy = manager->enabled_lights[index];
		entity_remove_light_component(manager, entity);

		map_put(&manager->disabled_id_map, entity.id, manager->disabled_count);
		

		if (stb_sb_count(manager->disabled_lights) == manager->disabled_count) {
			stb_sb_push(manager->disabled_lights, light_copy);
		} else {
			manager->disabled_lights[manager->disabled_count] = light_copy;
		}

		manager->disabled_count++;
		
	}
}

bool is_light_component_enabled(LightManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->enabled_id_map, entity.id);

	if (result.found) {
		return true;
	}
	return false;
}