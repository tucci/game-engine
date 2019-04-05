#include "Core/ECS/Component/MetaInfo.h"

#include "Common/stretchy_buffer.h"

void init_metainfo_manager(MetaInfoManager* manager) {
	map_init(&manager->id_map);
	manager->capacity = 0;
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
	manager->capacity = 0;
	manager->enabled_count = 0;
}




bool entity_add_metainfo_component(MetaInfoManager* manager, Entity entity, String name) {
	//MapResult<u64> result = map_get(&manager->id_map, entity.id);
	//// There already a component, return early and do nothing
	//if (result.found) return false;
	//
	//map_put(&manager->id_map, entity.id, manager->count);
	//manager->count++;
	//stb_sb_push(manager->names, String(NULL, 0));
	//set_name(manager, entity, name);
	//
	//return true;


	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There already a component, return early and do nothing
	if (result.found) return false;



	u64 index;

	if (manager->enabled_count == manager->capacity && stb_sb_count(manager->names) != manager->enabled_count) {
		manager->names[manager->enabled_count] = String(NULL, 0);
		manager->entitys[manager->enabled_count] = entity;
		index = manager->enabled_count;
		
	} else {
		stb_sb_push(manager->names, String(NULL,0));
		stb_sb_push(manager->entitys, entity);
		index = stb_sb_count(manager->names) - 1;
		
	}


	String this_comp = manager->names[index];
	String first_disabled_comp = manager->names[manager->enabled_count];


	// swap the first disabled comp with the comp we are trying to enable
	manager->names[manager->enabled_count] = this_comp;
	manager->names[index] = first_disabled_comp;

	// Swap entitys
	Entity this_entity = manager->entitys[index];
	Entity first_disabled_entity = manager->entitys[manager->enabled_count];
	manager->entitys[manager->enabled_count] = this_entity;
	manager->entitys[index] = first_disabled_entity;

	// Update the entity id to index mapping
	map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	map_put(&manager->id_map, first_disabled_entity.id, index);


	
	manager->enabled_count++;
	manager->capacity++;

	set_name(manager, entity, name);

	assert(manager->enabled_count <= manager->capacity);

	return true;

}

bool entity_remove_metainfo_component(MetaInfoManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return false;

	//removing disabled components
	u64 index = result.value;

	if (index >= manager->enabled_count) {
		String this_comp = manager->names[index];
		String last_comp = manager->names[manager->capacity - 1];

		// swap the last at the current index we are removing from
		manager->names[index] = last_comp;

		// Swap entitys
		Entity last_entity = manager->entitys[manager->capacity - 1];
		manager->entitys[index] = last_entity;

		// Remove the entity from the index map
		map_remove(&manager->id_map, entity.id);
		if (entity.id != last_entity.id) {
			map_put(&manager->id_map, last_entity.id, index);
		}

		manager->capacity--;
		assert(manager->enabled_count <= manager->capacity);
		return true;
	}



	// Get the last in the list to swap with

	String this_comp = manager->names[index];
	String last_comp = manager->names[manager->enabled_count - 1];

	// swap the last at the current index we are removing from
	manager->names[index] = last_comp;

	// Swap entitys
	Entity last_entity = manager->entitys[manager->enabled_count - 1];
	manager->entitys[index] = last_entity;

	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
	if (entity.id != last_entity.id) {
		map_put(&manager->id_map, last_entity.id, index);
	}
	manager->enabled_count--;
	manager->capacity--;
	assert(manager->enabled_count <= manager->capacity);
	return true;
}


void enable_metainfo_component(MetaInfoManager* manager, Entity entity, bool enabled) {

	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;

	if (enabled) {


		String this_comp = manager->names[index];
		String first_disabled_comp = manager->names[manager->enabled_count];


		// swap the first disabled comp with the comp we are trying to enable
		manager->names[manager->enabled_count] = this_comp;
		manager->names[index] = first_disabled_comp;

		// Swap entitys
		Entity this_entity = manager->entitys[index];
		Entity first_disabled_entity = manager->entitys[manager->enabled_count];
		manager->entitys[manager->enabled_count] = this_entity;
		manager->entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(&manager->id_map, this_entity.id, manager->enabled_count);
		map_put(&manager->id_map, first_disabled_entity.id, index);

		manager->enabled_count++;
	} else {


		String this_comp = manager->names[index];
		String last_comp = manager->names[manager->enabled_count - 1];
		// swap the last at the current index we are removing from
		manager->names[index] = last_comp;
		manager->names[manager->enabled_count - 1] = this_comp;

		// Swap entitys
		Entity this_entity = manager->entitys[index];
		Entity first_disabled_entity = manager->entitys[manager->enabled_count - 1];
		manager->entitys[manager->enabled_count - 1] = this_entity;
		manager->entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(&manager->id_map, this_entity.id, manager->enabled_count - 1);
		map_put(&manager->id_map, first_disabled_entity.id, index);

		manager->enabled_count--;
	}
	assert(manager->enabled_count <= manager->capacity);

}


bool is_metainfo_component_enabled(MetaInfoManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);

	if (!result.found) {
		return false;
	}
	// Check if the index is less than the enabled index
	// Everything to the left of the enabeld count is enabled, everything to the right of it, is disabled
	assert(manager->enabled_count <= manager->capacity);
	return result.value < manager->enabled_count;

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