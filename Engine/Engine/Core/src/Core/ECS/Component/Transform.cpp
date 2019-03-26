#pragma once

#include "Core/ECS/Component/Transform.h"

#include "Common/stretchy_buffer.h"

void init_transform_manager(TransformManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	
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
	manager->child_count = NULL;
	//manager->prev_sibling = NULL;

	
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
	stb_sb_free(manager->child_count);
	//stb_sb_free(manager->prev_sibling);

	map_destroy(&manager->id_map);
	
}

void entity_add_transform_component(TransformManager* manager, Entity entity) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;

	

	
	stb_sb_push(manager->positions, Vec3f(0, 0, 0));

	stb_sb_push(manager->scales, Vec3f(1, 1, 1));
	stb_sb_push(manager->ups, Vec3f(0, 1, 0));
	stb_sb_push(manager->forwards, Vec3f(0, 0, 1));
	stb_sb_push(manager->rights, Vec3f(1, 0, 0));
	stb_sb_push(manager->rotations, Quat());
	stb_sb_push(manager->local, Mat4x4f());
	stb_sb_push(manager->world, Mat4x4f());
	stb_sb_push(manager->parent, Entity());
	stb_sb_push(manager->first_child, Entity());
	stb_sb_push(manager->next_sibling, Entity());
	stb_sb_push(manager->child_count, 0);
	//stb_sb_push(manager->prev_sibling, Entity());



	
	
}

void entity_remove_transform_component(TransformManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;

	// Get the last in the list to swap with
	Vec3f last_pos = manager->positions[manager->count - 1];
	Vec3f last_scale = manager->scales[manager->count - 1];
	Vec3f last_up = manager->ups[manager->count - 1];
	Vec3f last_forward = manager->forwards[manager->count - 1];
	Vec3f last_right = manager->rights[manager->count - 1];
	Quat last_rotation = manager->rotations[manager->count - 1];

	Mat4x4f last_local = manager->local[manager->count - 1];
	Mat4x4f last_world = manager->world[manager->count - 1];

	Entity last_parent = manager->parent[manager->count - 1];
	Entity last_first_child = manager->first_child[manager->count - 1];
	Entity last_next_sibling = manager->next_sibling[manager->count - 1];

	u64 last_child_count = manager->child_count[manager->count - 1];

	// Swap
	manager->positions[index] = last_pos;
	manager->scales[index] = last_scale;
	manager->ups[index] = last_up;
	manager->forwards[index] = last_forward;
	manager->rights[index] = last_right;
	manager->rotations[index] = last_rotation;

	manager->local[index] = last_local;
	manager->world[index] = last_world;

	manager->parent[index] = last_parent;
	manager->first_child[index] = last_first_child;
	manager->next_sibling[index] = last_next_sibling;

	manager->child_count[index] = last_child_count;


	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
}

