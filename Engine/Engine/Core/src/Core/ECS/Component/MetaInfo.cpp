
#include "Core/ECS/Component/ComponentHelpers.h"
#include "Core/ECS/Component/MetaInfo.h"
#include "Common/stretchy_buffer.h"

void init_metainfo_manager(MetaInfoManager* manager) {
	map_init(&manager->id_map);
	manager->total_count = 0;
	manager->enabled_count = 0;
	manager->names = NULL;
	manager->entitys = NULL;

	arena_init(&manager->name_arena);
}

void destroy_metainfo_manager(MetaInfoManager* manager) {
	stb_sb_free(manager->names);
	stb_sb_free(manager->entitys);
	arena_free(&manager->name_arena);

	map_destroy(&manager->id_map);
	manager->total_count = 0;
	manager->enabled_count = 0;
}




bool entity_add_metainfo_component(MetaInfoManager* manager, Entity entity, String name) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There already a component, return early and do nothing
	if (result.found) return false;


	u64 index;
	u64 count = stb_sb_count(manager->entitys);
	if (manager->enabled_count == manager->total_count && count != manager->enabled_count) {
		manager->entitys[manager->enabled_count] = entity;
		index = manager->enabled_count;
	} else {
		stb_sb_push(manager->entitys, entity);
		index = stb_sb_count(manager->entitys) - 1;
	}





	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->names, name, index);

	// Swap entitys
	Entity this_entity = manager->entitys[index];
	Entity first_disabled_entity = manager->entitys[manager->enabled_count];
	manager->entitys[manager->enabled_count] = this_entity;
	manager->entitys[index] = first_disabled_entity;

	// Update the entity id to index mapping
	map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	map_put(&manager->id_map, first_disabled_entity.id, index);

	manager->enabled_count++;
	manager->total_count++;

	assert(manager->enabled_count <= manager->total_count);

	return true;

}

bool entity_remove_metainfo_component(MetaInfoManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return false;


	u64 index = result.value;
	u64 index_to_swap;

	//removing disabled components
	if (index >= manager->enabled_count) {
		index_to_swap = manager->total_count - 1;
	} else {
		index_to_swap = manager->enabled_count - 1;
	}


	comphelper_remove_component_data(entity, manager->names, index, index_to_swap);

	Entity last_entity = manager->entitys[index_to_swap];
	manager->entitys[index] = last_entity;

	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
	if (entity.id != last_entity.id) {
		map_put(&manager->id_map, last_entity.id, index);
	}

	manager->enabled_count--;
	manager->total_count--;
	assert(manager->enabled_count <= manager->total_count);

	return true;
}


void enable_metainfo_component(MetaInfoManager* manager, Entity entity, bool enabled) {

	comphelper_enable_component(&manager->id_map, manager->entitys, &manager->enabled_count, manager->names, entity, enabled);
	assert(manager->enabled_count <= manager->total_count);

}


bool is_metainfo_component_enabled(MetaInfoManager* manager, Entity entity) {
	// Check if the index is less than the enabled index
	// Everything to the left of the enabeld count is enabled, everything to the right of it, is disabled
	assert(manager->enabled_count <= manager->total_count);

	return comphelper_is_component_enabled(&manager->id_map, &manager->enabled_count, entity);

}


void set_name(MetaInfoManager* manager, Entity entity, String name) {

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	assert(result.found);
	if (!result.found) {}
	int index = result.value;

	String* e_name = &manager->names[index];
	if (e_name->buffer == NULL) {
		// First time nameing
	} else {
		// This is a rename
		// we need to free up the old name
	}



	// +1 to length to null terminate it in memory
	char* name_copy = (char*)arena_alloc(&manager->name_arena, name.length + 1);
	memcpy(name_copy, name.buffer, name.length);
	// add the null terminator to end of string inside the allocator memory
	name_copy[name.length] = '\0';
	
	e_name->length = name.length;
	e_name->buffer = name_copy;


}

String get_name(MetaInfoManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	assert(result.found);
	if (!result.found) {}
	int index = result.value;

	return manager->names[index];
}