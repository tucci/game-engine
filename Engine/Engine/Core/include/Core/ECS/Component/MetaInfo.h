#pragma once

#include "types.h"
#include "Common/Map.h"
#include "Common/Arena.h"

#include "Core/ECS/Entity.h"


struct MetaInfoManager {
	u64 count;
	CompactMap<u64> id_map;

	String* names;

	Arena name_arena;
};


void init_metainfo_manager(MetaInfoManager* manager);
void destroy_metainfo_manager(MetaInfoManager* manager);

bool entity_add_metainfo_component(MetaInfoManager* manager, Entity entity, String name);
bool entity_remove_metainfo_component(MetaInfoManager* manager, Entity entity);

void set_name(MetaInfoManager* manager, Entity entity, String name);
String get_name(MetaInfoManager* manager, Entity entity);