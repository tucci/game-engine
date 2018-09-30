#pragma once

#include "Core/ECS/JobSystem/LightSystem.h"


Light get_light(EntityManager* manager, Entity entity) {
	uint64_t index = map_get(&manager->lights.id_map, entity.id);
	return manager->lights.lights[index];
}

void set_light(EntityManager* manager, Entity entity, Light light) {
	uint64_t index = map_get(&manager->lights.id_map, entity.id);
	manager->lights.lights[index] = light;
}