#pragma once

#include "Core/ECS/JobSystem/CameraSystem.h"

Camera* get_camera(EntityManager* manager, Entity entity) {
	uint64_t index = map_get(&manager->cameras.id_map, entity.id);
	return &manager->cameras.cameras[index];
}