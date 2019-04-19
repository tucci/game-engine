#pragma once

#include "Core/ECS/Component/ComponentHelpers.h"
#include "Core/ECS/Component/Camera.h"

#include "Common/common_macros.h"
#include "Common/stretchy_buffer.h"



void init_camera_params(Camera* camera,
	float near_clip,
	float far_clip,
	float fov,
	float aspect_ratio) {

	camera->near_clip = near_clip;
	camera->far_clip = far_clip;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}




void init_camera_manager(CameraManager* manager) {
	map_init(&manager->id_map);
	manager->enabled_count = 0;
	manager->total_count = 0;
	manager->enabled_cameras = NULL;
	manager->entitys = NULL;

}
void destroy_camera_manager(CameraManager* manager) {

	stb_sb_free(manager->enabled_cameras);
	stb_sb_free(manager->entitys);
	map_destroy(&manager->id_map);
	manager->total_count = 0;
	manager->enabled_count = 0;
}

bool entity_add_camera_component(CameraManager* manager, Entity entity) {

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





	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->enabled_cameras, Camera(entity), index);

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

bool entity_remove_camera_component(CameraManager* manager, Entity entity) {
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


	comphelper_remove_component_data(entity, manager->enabled_cameras, index, index_to_swap);

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

void enable_camera_component(CameraManager* manager, Entity entity, bool enabled) {

}