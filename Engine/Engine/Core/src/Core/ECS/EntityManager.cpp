#pragma once

#include "Core/ECS/EntityManager.h"
#include "Common/common_macros.h"



#include "Common/stretchy_buffer.h"


Entity create_entity(EntityManager* manager, String name) {
	
	
	int entity_index = manager->entitys_created;
	manager->entitys_created++;
	Entity entity;
	// Note our entites shouldnt start at 0, because our map expects that no key maps to 0
	entity.id = entity_index + 1;
	if (stb_sb_count(manager->entity_list) == manager->entity_count) {
		stb_sb_push(manager->entity_list, entity);
	} else {
		manager->entity_list[manager->entity_count] = entity;
	}
	

	// adds a mapping between the entity's id, and its index
	// while it seems dumb that the key,values are obvious when adding
	// when we delete an entity, we may need to remap its index to something else
	// mapping <entity_id, index into the entity array>
	map_put(&manager->entity_index_map, entity.id, manager->entity_count);
	manager->entity_count++;

	entity_add_transform_component(&manager->transform_manager, entity, name);
	

	return entity;
	

	
}

void destroy_entity(EntityManager* manager, Entity entity) {
	
	
	// Entity list before deleting [1, 2, 3, 4, 5]
	//                                          ^
	//                                     entity_count
	// Delete entity 3
	// Entity list after swapping [1, 2, 5, 4, 3]
	// Entity list after deleting [1, 2, 5, 4][5]
	//                                      ^
	//                                entity_count

	// Notice how [5] is still in the list at the end, we could have swapped 3 and 5
	// but it is not needed because we know how many entites we have
	// Note we are not shrinking the entity list array. it just keeps expanding untill the game/engines exits
	// because we are keeping track of how many items we have in the list
	// we differentiate between the array size, and how many entities are in the array




	
	
	// Get index of the entity we are trying to remove
	MapResult<u64> result = map_get(&manager->entity_index_map, entity.id);
	if (!result.found) {
		// This entity has already been deleted 
		return;
	}
	u64 index = result.value;
	
	Entity last_entity_in_list_to_swap_with = manager->entity_list[manager->entity_count - 1];
	// Place the last entity in the entity list at the index we are removing from
	manager->entity_list[index] = last_entity_in_list_to_swap_with;
	manager->entity_count--;
	// Update the mapping of the swapped entity to it's new index
	map_put(&manager->entity_index_map, last_entity_in_list_to_swap_with.id, index);
	map_remove(&manager->entity_index_map, entity.id);
	

	// Need to remove it from all the other components

	for (int i = (int)ComponentType::None; i < (int)ComponentType::Count; i++) {
		ComponentType type = (ComponentType)i;
		remove_component(manager, entity, type);
	}

	
	
}

void add_component(EntityManager* manager, Entity entity, ComponentType type) {

	// Get index of the entity we are trying to remove
	MapResult<u64> result = map_get(&manager->entity_index_map, entity.id);
	if (!result.found) {
		// This entity has already been deleted 
		return;
	}

	switch (type) {
		case ComponentType::Transform: {
			//entity_add_transform_component(&manager->transform_manager, entity);
			break;
		}
		case ComponentType::Camera: {
			entity_add_camera_component(&manager->camera_manager, entity);
			
			break;
		}
		case ComponentType::StaticMesh: {
			entity_add_mesh_component(&manager->static_mesh_manger, entity);
			break;
		}
		case ComponentType::Light: {
			entity_add_light_component(&manager->light_manager, entity);
			break;
		}

		case ComponentType::Render: {
			entity_add_render_component(&manager->render_manager, entity);
			break;
		}
	}
}


void remove_component(EntityManager* manager, Entity entity, ComponentType type) {
	switch (type) {
		case ComponentType::Transform: {
			entity_remove_transform_component(&manager->transform_manager, entity);
			break;
		}
		case ComponentType::Camera: {
			entity_remove_camera_component(&manager->camera_manager, entity);
			break;
		}
		case ComponentType::StaticMesh: {
			entity_remove_mesh_component(&manager->static_mesh_manger, entity);
			break;
		}
		case ComponentType::Light: {
			entity_remove_light_component(&manager->light_manager, entity);
			break;
		}

		case ComponentType::Render: {
			entity_remove_render_component(&manager->render_manager, entity);
			break;
		}
	}
}


void init_entity_manager(EntityManager* manager) {
	arena_init(&manager->arena);


	manager->entity_count = 0;
	manager->entitys_created = 0;
	manager->entity_list = NULL;
	map_init(&manager->entity_index_map);


	size_t mem_size = ECS_MEMORY;
	void* mem_block = arena_alloc(&manager->arena, mem_size);
	mem_size = manager->arena.end - cast(char*) mem_block;
	stack_alloc_init(&manager->stack_mem, mem_block, mem_size);
	
	
	init_transform_manager(&manager->transform_manager);
	init_static_mesh_manager(&manager->static_mesh_manger);
	init_camera_manager(&manager->camera_manager);
	init_light_manager(&manager->light_manager);
	init_render_manager(&manager->render_manager);

	

}

void destroy_entity_manager(EntityManager* manager) {
	arena_free(&manager->arena);
	stb_sb_free(manager->entity_list);

	
	destroy_transform_manager(&manager->transform_manager);
	destroy_static_mesh_manager(&manager->static_mesh_manger);
	destroy_camera_manager(&manager->camera_manager);
	destroy_light_manager(&manager->light_manager);
	destroy_render_manager(&manager->render_manager);
}

void attach_child_entity(EntityManager* manager, Entity entity, Entity child) {

	if (entity.id == child.id) {
		// An entity can't be a child of itself
		return;
	}

	// This entity was not found. 
	// Just do nothing, and return back
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { return; }
	
	
	// Get the mapped index into the first child array
	
	u64 entity_index = result.value;

	Entity* first_child = &manager->transform_manager.first_child[entity_index];
	if (first_child->id == NO_ENTITY_ID) {
		// This entity has no children, so we need to add this child to the first child
		first_child->id = child.id;
	} else {
		// This entity already has a first child so we need to look at the siblings
		// Try to get the mapped index for the child entity
		MapResult<u64> child_result = map_get(&manager->transform_manager.id_map, first_child->id);
		// This index is not in the manager
		if (!child_result.found) { return; }
		u64 first_child_index = child_result.value;

		// Get the next sibling entity mapped to the child index by using the next_sibling array
		Entity* next_sibling = &manager->transform_manager.next_sibling[first_child_index];


		// Loop over all the siblings untill we get to the end of the list
		while (next_sibling->id != NO_ENTITY_ID) {
			// Get next sibling
			child_result = map_get(&manager->transform_manager.id_map, next_sibling->id);
			// if the entity is not valid do nothing
			if (!child_result.found) { return; }
			// move the next_sibling entity to the next one
			next_sibling = &manager->transform_manager.next_sibling[child_result.value];
		}
		next_sibling->id = child.id;
		
	}
	MapResult<u64> child_result = map_get(&manager->transform_manager.id_map, child.id);
	if (!child_result.found) {
		return;
	}

	// Assign the child, the parent entity
	u64 child_index = child_result.value;
	
	manager->transform_manager.parent[child_index] = entity;
	manager->transform_manager.child_count[entity_index]++;
}

u64 entity_count(EntityManager* manager) {
	u64 count = manager->entity_count;
	return count;
}

Entity parent(EntityManager* manager, Entity entity) {
	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) {
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	Entity parent_entity = manager->transform_manager.parent[index];
	return parent_entity;
}


Entity first_child(EntityManager* manager, Entity entity) {

	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { 
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	// Get the first child for this entity
	Entity first = manager->transform_manager.first_child[index];
	return first;
}

Entity next_sibling(EntityManager* manager, Entity entity) {
	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) {
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	// Get the next sibling for this entity
	Entity next = manager->transform_manager.next_sibling[index];
	return next;
}

u64 children_count(EntityManager* manager, Entity entity) {
	// This entity was not found. 
	// Just do nothing, and return back
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { return 0; }
	// Get the mapped index into the first child array
	u64 entity_index = result.value;

	u64 count = manager->transform_manager.child_count[entity_index];
	return count;
}

u64 get_index_for_entity(EntityManager* manager, Entity entity, CompactMap<u64>* map) {
	MapResult<u64> result = map_get(map, entity.id);
	assert(result.found);
	if (!result.found) {}
	int index = result.value;
	return index;
}

