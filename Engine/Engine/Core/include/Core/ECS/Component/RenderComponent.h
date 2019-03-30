#pragma once

#include "Common/Map.h"
#include "Core/ECS/Entity.h"
#include "Asset/Asset.h"

struct Render {
	Entity entity_ref;
	MaterialID material_id;
	Render(Entity e) {
		entity_ref = e; 
		material_id.id = 0;
	}
	

};

struct RenderManager {
	CompactMap<u64> enabled_id_map;
	CompactMap<u64> disabled_id_map;

	Render* enabled_renders;
	u64 enabled_count;

	Render* disabled_renders;
	u64 disabled_count;
};


void init_render_manager(RenderManager* manager);
void destroy_render_manager(RenderManager* manager);

bool entity_add_render_component(RenderManager* manager, Entity entity);
bool entity_remove_render_component(RenderManager* manager, Entity entity);

void enable_render_component(RenderManager* manager, Entity entity, bool enabled);
bool is_render_component_enabled(RenderManager* manager, Entity entity);