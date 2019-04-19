#pragma once


#include "types.h"
#include "Common/Map.h"
#include "Common/Arena.h"

#include "Core/ECS/Entity.h"


struct MetaInfoManager {
	
	CompactMap<u64> id_map;

	Entity* entitys;
	String* names;

	u64 total_count;
	u64 enabled_count;

	Arena name_arena;
};


void init_metainfo_manager(MetaInfoManager* manager);
void destroy_metainfo_manager(MetaInfoManager* manager);

bool entity_add_metainfo_component(MetaInfoManager* manager, Entity entity, String name);
bool entity_remove_metainfo_component(MetaInfoManager* manager, Entity entity);

void enable_metainfo_component(MetaInfoManager* manager, Entity entity, bool enabled);
bool is_metainfo_component_enabled(MetaInfoManager* manager, Entity entity);



void set_name(MetaInfoManager* manager, Entity entity, String name);
String get_name(MetaInfoManager* manager, Entity entity);