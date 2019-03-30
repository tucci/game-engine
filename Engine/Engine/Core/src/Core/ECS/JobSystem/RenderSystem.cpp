#pragma once

#include "Core/ECS/JobSystem/RenderSystem.h"



void set_render_material(EntityManager* manager, Entity entity, MaterialID id) {
	int index = get_index_for_entity(manager, entity, &manager->render_manager.enabled_id_map);
	manager->render_manager.enabled_renders[index].material_id = id;
}


MaterialID get_render_material(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->render_manager.enabled_id_map, entity.id);
	if (!result.found) {
		MaterialID id;
		id.id = 0;
		return id;
	}
	u64 index = result.value;
	MaterialID id;
	id = manager->render_manager.enabled_renders[index].material_id;
	return id;
}