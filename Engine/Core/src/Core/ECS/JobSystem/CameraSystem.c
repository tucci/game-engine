#pragma once

#include "Core/ECS/JobSystem/CameraSystem.h"

Camera* get_camera(EntityManager* manager, Entity entity) {
	MapResult<uint64_t> result = map_get(&manager->cameras.id_map, entity.id);
	if (!result.found) return NULL;
	uint64_t index = result.value;
	return &manager->cameras.cameras[index];
}