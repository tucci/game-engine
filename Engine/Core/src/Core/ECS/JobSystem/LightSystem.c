#pragma once

#include "Core/ECS/JobSystem/LightSystem.h"


Light get_light(EntityManager* manager, Entity entity) {
	MapResult<uint64_t> result = map_get(&manager->light_manager.id_map, entity.id);
	if (!result.found) {
		Light no_light;
		no_light.type = LightType_None;
		return no_light;
	}
	uint64_t index = result.value;
	return manager->light_manager.lights[index];
}

bool set_light(EntityManager* manager, Entity entity, Light light) {
	MapResult<uint64_t> result = map_get(&manager->light_manager.id_map, entity.id);
	if (!result.found) {
		return false;
	}
	uint64_t index = result.value;
	manager->light_manager.lights[index] = light;
	return true;
}