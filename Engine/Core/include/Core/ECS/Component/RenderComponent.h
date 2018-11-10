#pragma once

#include "Common/Map.h"
#include "Core/ECS/Entity.h"

typedef struct Render {
	Entity entity_ref;
	bool visible;
	Render(Entity e, bool visibility) {
		entity_ref = e; 
		visible = visibility;
	}

}Render;

typedef struct RenderManager {
	CompactMap<uint64_t> id_map;
	Render* renders;
	uint64_t count;
} RenderManager;


void init_render_manager(RenderManager* manager);
void destroy_render_manager(RenderManager* manager);

void entity_add_render_component(RenderManager* manager, Entity entity);
void entity_remove_render_component(RenderManager* manager, Entity entity);