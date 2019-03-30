#pragma once

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
	manager->enabled_cameras = NULL;

}
void destroy_camera_manager(CameraManager* manager) {

	stb_sb_free(manager->enabled_cameras);
	map_destroy(&manager->id_map);
}

bool entity_add_camera_component(CameraManager* manager, Entity entity) {

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There already a component, return early and do nothing
	if (result.found) return false;

	if (stb_sb_count(manager->enabled_cameras) == manager->enabled_count) {
		stb_sb_push(manager->enabled_cameras, Camera(entity));
	} else {
		manager->enabled_cameras[manager->enabled_count] = Camera(entity);
	}
	map_put(&manager->id_map, entity.id, manager->enabled_count);
	manager->enabled_count++;

	return true;
	
}

bool entity_remove_camera_component(CameraManager* manager, Entity entity) {
	// See if this entity even has a camera
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return false;
	
	u64 index = result.value;
	// Get the last camera in the list to swap with
	Camera last = manager->enabled_cameras[manager->enabled_count - 1];
	// swap the last camera at the current index we are removing from
	manager->enabled_cameras[index] = last;
	manager->enabled_count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
	return true;
}

void enable_camera_component(CameraManager* manager, Entity entity, bool enabled) {

}