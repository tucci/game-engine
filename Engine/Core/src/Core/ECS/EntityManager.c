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
			map_put(&manager->transforms.id_map, entity.id, manager->transforms.count);
			manager->transforms.count++;
			

			stb_sb_push(manager->transforms.positions, Vec3f(0,0,0));
			stb_sb_push(manager->transforms.scales, Vec3f(1, 1, 1));
			stb_sb_push(manager->transforms.ups, Vec3f(0, 1, 0));
			stb_sb_push(manager->transforms.forwards, Vec3f(0, 0, 1));
			stb_sb_push(manager->transforms.rights, Vec3f(1, 0, 0));
			stb_sb_push(manager->transforms.rotations, Quat());
			stb_sb_push(manager->transforms.local, Mat4x4f());
			stb_sb_push(manager->transforms.world, Mat4x4f());
			stb_sb_push(manager->transforms.parent, Entity());
			stb_sb_push(manager->transforms.first_child, Entity());
			stb_sb_push(manager->transforms.next_sibling, Entity());
			//stb_sb_push(manager->transforms.prev_sibling, NO_ENTITY_ID);
			break;
		}
		case ComponentType_Camera: {
			map_put(&manager->cameras.id_map, entity.id, manager->cameras.count);
			manager->cameras.count++;
			stb_sb_push(manager->cameras.cameras, Camera());
			break;
		}
		case ComponentType_StaticMesh: {
			map_put(&manager->static_meshs.id_map, entity.id, manager->static_meshs.count);
			manager->static_meshs.count++;
			stb_sb_push(manager->static_meshs.meshes, StaticMesh());
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
	
	map_init(&manager->transforms.id_map);
	manager->transforms.count = 0;
	manager->transforms.positions = NULL;
	manager->transforms.scales = NULL;
	manager->transforms.ups = NULL;
	manager->transforms.forwards = NULL;
	manager->transforms.rights = NULL;
	manager->transforms.rotations = NULL;
	manager->transforms.local = NULL;
	manager->transforms.world = NULL;
	manager->transforms.parent = NULL;
	manager->transforms.first_child = NULL;
	manager->transforms.next_sibling = NULL;
	//manager->transforms.prev_sibling = NULL;
	
	
	
	// For now every entity has every component to test
	//manager->transforms.count = entity_count;
	//manager->transforms.positions = cast(Vec3f*) stack_alloc(&manager->stack_mem, sizeof(Vec3f) * entity_count, 1);
	//manager->transforms.scales = cast(Vec3f*) stack_alloc(&manager->stack_mem, sizeof(Vec3f) * entity_count, 1);
	//manager->transforms.rotations = cast(Quat*) stack_alloc(&manager->stack_mem, sizeof(Quat) * entity_count, 1);
	//manager->transforms.forwards = cast(Vec3f*) stack_alloc(&manager->stack_mem, sizeof(Vec3f) * entity_count, 1);
	//manager->transforms.ups = cast(Vec3f*) stack_alloc(&manager->stack_mem, sizeof(Vec3f) * entity_count, 1);
	//manager->transforms.rights = cast(Vec3f*) stack_alloc(&manager->stack_mem, sizeof(Vec3f) * entity_count, 1);
	//manager->transforms.local = cast(Mat4x4f*) stack_alloc(&manager->stack_mem, sizeof(Mat4x4f) * entity_count, 1);
	//manager->transforms.world = cast(Mat4x4f*) stack_alloc(&manager->stack_mem, sizeof(Mat4x4f) * entity_count, 1);
	//manager->transforms.parent = cast(int*) stack_alloc(&manager->stack_mem, sizeof(int) * entity_count, 1);
	//manager->transforms.first_child = cast(int*) stack_alloc(&manager->stack_mem, sizeof(int) * entity_count, 1);
	//manager->transforms.next_sibling = cast(int*) stack_alloc(&manager->stack_mem, sizeof(int) * entity_count, 1);
	//manager->comp_manager.transform_manager.prev_sibling = cast(int*) stack_alloc(&manager->mem, sizeof(int) * entity_count, 1);
	
	map_init(&manager->cameras.id_map);
	manager->cameras.count = 0;
	manager->cameras.cameras = NULL;
	//manager->cameras.cameras = cast(Camera*) stack_alloc(&manager->stack_mem, sizeof(Camera) * entity_count, 1);

	map_init(&manager->static_meshs.id_map);
	manager->static_meshs.count = 0;
	manager->static_meshs.meshes = NULL;
	//manager->static_meshs.meshes = cast(StaticMesh*) stack_alloc(&manager->stack_mem, sizeof(StaticMesh) * entity_count, 1);


	

}

void destroy_entity_manager(EntityManager* manager) {
	arena_free(&manager->arena);
	stb_sb_free(manager->entity_list);

	stb_sb_free(manager->transforms.positions);
	stb_sb_free(manager->transforms.scales);
	stb_sb_free(manager->transforms.rotations);
	stb_sb_free(manager->transforms.ups);
	stb_sb_free(manager->transforms.forwards);
	stb_sb_free(manager->transforms.rights);
	stb_sb_free(manager->transforms.local);
	stb_sb_free(manager->transforms.world);
	stb_sb_free(manager->transforms.parent);
	stb_sb_free(manager->transforms.first_child);
	stb_sb_free(manager->transforms.next_sibling);
	//stb_sb_free(manager->transforms.prev_sibling);

	stb_sb_free(manager->cameras.cameras);
	stb_sb_free(manager->static_meshs.meshes);

	map_destroy(&manager->transforms.id_map);
	map_destroy(&manager->cameras.id_map);
	map_destroy(&manager->static_meshs.id_map);
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

Camera* get_camera(EntityManager* manager, Entity entity) {
	uint64_t index = map_get(&manager->cameras.id_map, entity.id);
	return &manager->cameras.cameras[index];
}

StaticMesh* get_static_mesh(EntityManager* manager, Entity entity) {
	uint64_t index = map_get(&manager->static_meshs.id_map, entity.id);
	return &manager->static_meshs.meshes[index];
}

void set_static_mesh(EntityManager* manager, Entity entity, StaticMesh* mesh) {
	uint64_t index = map_get(&manager->static_meshs.id_map, entity.id);
	manager->static_meshs.meshes[index] = *mesh;
}
