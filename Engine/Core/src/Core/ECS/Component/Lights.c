#pragma once

#include "Core/ECS/Component/Lights.h"
#include "Common/stretchy_buffer.h"

void init_light_manager(LightManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->lights = NULL;
}

void destroy_light_manager(LightManager* manager) {
	stb_sb_free(manager->lights);
	map_destroy(&manager->id_map);
}
void entity_add_light_component(LightManager* manager, Entity entity) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->lights, Light());
}