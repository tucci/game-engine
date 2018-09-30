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
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->cameras, Camera());
}