#pragma once

#include "Common/Map.h"
#include "Core/ECS/Entity.h"
#include "Asset/Asset.h"

struct Render {
	Entity entity_ref;
	bool visible;
	MaterialID material_id;
	Render(Entity e, bool visibility) {
		entity_ref = e; 
		visible = visibility;
		material_id.id = 0;
	}
	

};

struct RenderManager {
	CompactMap<u64> id_map;
	Render* renders;
	u64 count;
};


void init_render_manager(RenderManager* manager);
void destroy_render_manager(RenderManager* manager);

void entity_add_render_component(RenderManager* manager, Entity entity);
void entity_remove_render_component(RenderManager* manager, Entity entity);