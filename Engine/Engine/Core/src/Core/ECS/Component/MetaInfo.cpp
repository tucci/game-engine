#include "Core/ECS/Component/MetaInfo.h"

#include "Common/stretchy_buffer.h"

void init_metainfo_manager(MetaInfoManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->names = NULL;

	arena_init(&manager->name_arena);
}

void destroy_metainfo_manager(MetaInfoManager* manager) {
	stb_sb_free(manager->names);
	arena_free(&manager->name_arena);
}




void entity_add_metainfo_component(MetaInfoManager* manager, Entity entity, String name) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->names, String(NULL, 0));
	set_name(manager, entity, name);
}

void entity_remove_metainfo_component(MetaInfoManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;

	String last_name = manager->names[manager->count - 1];
	// Swap
	manager->names[index] = last_name;

	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
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