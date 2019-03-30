#pragma once

#include "Core/ECS/JobSystem/LightSystem.h"


Light get_light(EntityManager* manager, Entity entity) {

	LightManager* lm = &manager->light_manager;
	// Check in the enabled lights first
	MapResult<u64> result = map_get(&lm->enabled_id_map, entity.id);
	if (result.found) {
		u64 index = result.value;
		Light light = lm->enabled_lights[index];
		return light;
	}
	// If the component wasn't found inside the enabled list
	// it might be inside the disabled list
	
	result = map_get(&lm->disabled_id_map, entity.id);
	if (result.found) {
		u64 index = result.value;
		Light light = lm->disabled_lights[index];
		return light;
	}
	
	Light no_light;
	no_light.type = LightType::None;
	return no_light;

}

bool set_light(EntityManager* manager, Entity entity, Light light) {
	//MapResult<u64> result = map_get(&manager->light_manager.id_map, entity.id);
	//if (!result.found) {
	//	return false;
	//}
	//u64 index = result.value;
	//manager->light_manager.lights[index] = light;
	//return true;

	LightManager* lm = &manager->light_manager;
	// Check in the enabled lights first
	MapResult<u64> result = map_get(&lm->enabled_id_map, entity.id);
	if (result.found) {
		u64 index = result.value;
		lm->enabled_lights[index] = light;
		return true;
	}

	// If the component wasn't found inside the enabled list
	// it might be inside the disabled list

	result = map_get(&lm->disabled_id_map, entity.id);
	if (result.found) {
		u64 index = result.value;
		lm->disabled_lights[index] = light;
		return true;
	}
	return false;
}