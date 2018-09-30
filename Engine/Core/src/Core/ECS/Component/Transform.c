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
	//stb_sb_push(manager->prev_sibling, Entity());
}