#pragma once

#include "Core/ECS/JobSystem/CameraSystem.h"

Camera* get_camera(EntityManager* manager, Entity entity) {
	MapResult<uint64_t> result = map_get(&manager->camera_manager.id_map, entity.id);
	if (!result.found) return NULL;
	uint64_t index = result.value;
	return &manager->camera_manager.cameras[index];
}