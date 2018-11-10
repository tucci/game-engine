#pragma once

#include "Core/ECS/Component/Camera.h"

#include "Common/common_macros.h"
#include "Common/stretchy_buffer.h"



void init_camera_params(Camera* camera,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}




void init_camera_manager(CameraManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->cameras = NULL;

}
void destroy_camera_manager(CameraManager* manager) {

	stb_sb_free(manager->cameras);
	map_destroy(&manager->id_map);
}
void entity_add_camera_component(CameraManager* manager, Entity entity) {
	if (stb_sb_count(manager->cameras) == manager->count) {
		stb_sb_push(manager->cameras, Camera(entity));
	} else {
		manager->cameras[manager->count] = Camera(entity);
	}

	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	
}
void entity_remove_camera_component(CameraManager* manager, Entity entity) {
	// See if this entity even has a camera
	MapResult<uint64_t> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;
	
	uint64_t index = result.value;
	// Get the last camera in the list to swap with
	Camera last = manager->cameras[manager->count - 1];
	// swap the last camera at the current index we are removing from
	manager->cameras[index] = last;
	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
}