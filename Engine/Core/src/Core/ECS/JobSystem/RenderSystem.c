#pragma once

#include "Core/ECS/JobSystem/RenderSystem.h"

void set_render_visibility(EntityManager* manager, Entity entity, bool visibility) {
	int index = get_index_for_entity(manager, entity, &manager->render_manager.id_map);
	manager->render_manager.renders[index].visible = visibility;
}