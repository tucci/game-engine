#pragma once

#include "Core/ECS/Component/ComponentHelpers.h"
#include "Core/ECS/Component/Transform.h"

#include "Common/stretchy_buffer.h"






void init_transform_manager(TransformManager* manager) {
	map_init(&manager->id_map);

	manager->total_count = 0;
	manager->enabled_count = 0;
	manager->entitys = NULL;
	
	manager->positions = NULL;
	manager->scales = NULL;
	manager->ups = NULL;
	manager->forwards = NULL;
	manager->rights = NULL;
	manager->rotations = NULL;
	manager->local = NULL;
	manager->world = NULL;
	manager->parent = NULL;
	manager->first_child = NULL;
	manager->next_sibling = NULL;
	manager->prev_sibling = NULL;
	manager->child_count = NULL;
	

	
}

void destroy_transform_manager(TransformManager* manager) {
	
	stb_sb_free(manager->positions);
	stb_sb_free(manager->scales);
	stb_sb_free(manager->rotations);
	stb_sb_free(manager->ups);
	stb_sb_free(manager->forwards);
	stb_sb_free(manager->rights);
	stb_sb_free(manager->local);
	stb_sb_free(manager->world);
	stb_sb_free(manager->parent);
	stb_sb_free(manager->first_child);
	stb_sb_free(manager->next_sibling);
	stb_sb_free(manager->prev_sibling);
	stb_sb_free(manager->child_count);

	map_destroy(&manager->id_map);
	
}

bool entity_add_transform_component(TransformManager* manager, Entity entity) {

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


	
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->positions, Vec3f(0,0,0), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->scales, Vec3f(1, 1, 1), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->ups, Vec3f(0, 1, 0), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->forwards, Vec3f(0, 0, 1), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->rights, Vec3f(1, 0, 0), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->rotations, Quat(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->local, Mat4x4f(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->world, Mat4x4f(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->parent, Entity(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->first_child, Entity(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->next_sibling, Entity(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->prev_sibling, Entity(), index);
	comphelper_add_component_data(&manager->total_count, &manager->enabled_count, entity, &manager->child_count, u64(0), index);
	
	
	// Swap entitys
	Entity this_entity = manager->entitys[index];
	Entity first_disabled_entity = manager->entitys[manager->enabled_count];
	manager->entitys[manager->enabled_count] = this_entity;
	manager->entitys[index] = first_disabled_entity;

	// Update the entity id to index mapping
	map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	map_put(&manager->id_map, first_disabled_entity.id, index);

	

	
	//map_put(&manager->id_map, this_entity.id, manager->enabled_count);
	//map_put(&manager->id_map, first_disabled_entity.id, index);

	manager->enabled_count++;
	manager->total_count++;

	assert(manager->enabled_count <= manager->total_count);

	return true;

}

bool entity_remove_transform_component(TransformManager* manager, Entity entity) {
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



	comphelper_remove_component_data(entity, manager->positions, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->scales, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->ups, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->forwards, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->rights, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->rotations, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->local, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->world, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->parent, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->first_child, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->next_sibling, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->prev_sibling, index, index_to_swap);
	comphelper_remove_component_data(entity, manager->child_count, index, index_to_swap);


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

