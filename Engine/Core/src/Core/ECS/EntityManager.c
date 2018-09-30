#pragma once

#include "Core/ECS/EntityManager.h"
#include "Common/common_macros.h"
#include "Core/ECS/JobSystem/TransformSystem.h"

#include "Common/stretchy_buffer.h"


Entity create_entity(EntityManager* manager) {
	
	int count = stb_sb_count(manager->entity_list);
	Entity e_temp;
	e_temp.id = count + 1;
	stb_sb_push(manager->entity_list, e_temp);
	Entity entity = stb_sb_last(manager->entity_list);
	return entity;
}

void add_component(EntityManager* manager, Entity entity, ComponentType type) {
	switch (type) {
		case ComponentType_Transform: {
			entity_add_transform_component(&manager->transforms, entity);
			break;
		}
		case ComponentType_Camera: {
			entity_add_camera_component(&manager->cameras, entity);
			break;
		}
		case ComponentType_StaticMesh: {
			entity_add_mesh_component(&manager->static_meshs, entity);
			break;
		}
		case ComponentType_Light: {
			entity_add_light_component(&manager->lights, entity);
			break;
		}
	}
}


void init_entity_manager(EntityManager* manager) {
	arena_init(&manager->arena);


	manager->entity_list = NULL;

	size_t mem_size = ECS_MEMORY;
	void* mem_block = arena_alloc(&manager->arena, mem_size);
	mem_size = manager->arena.end - cast(char*) mem_block;
	stack_alloc_init(&manager->stack_mem, mem_block, mem_size);
	
	
	init_transform_manager(&manager->transforms);
	init_static_mesh_manager(&manager->static_meshs);
	init_camera_manager(&manager->cameras);
	init_light_manager(&manager->lights);

	

}

void destroy_entity_manager(EntityManager* manager) {
	arena_free(&manager->arena);
	stb_sb_free(manager->entity_list);

	
	destroy_transform_manager(&manager->transforms);
	destroy_static_mesh_manager(&manager->static_meshs);
	destroy_camera_manager(&manager->cameras);
	destroy_light_manager(&manager->lights);
}

void attach_child_entity(EntityManager* manager, Entity entity, Entity child) {
	
	
	uint64_t index = map_get(&manager->transforms.id_map, entity.id);

	Entity* first_child = &manager->transforms.first_child[index];
	if (first_child->id == NO_ENTITY_ID) {
		// This entity has no children, so we need to add this child to the first child
		first_child->id = child.id;
	} else {
		// This entity already has a first child so we need to look at the siblings
		Entity* next_sibling = &manager->transforms.next_sibling[first_child->id];

		while (next_sibling->id != NO_ENTITY_ID) {
			next_sibling = &manager->transforms.next_sibling[next_sibling->id];
		}
		next_sibling->id = child.id;
		
	}
	// Assign the child, the parent entity
	uint64_t child_index = map_get(&manager->transforms.id_map, child.id);
	
	manager->transforms.parent[child_index] = entity;
}

