#pragma once

#include "Core/ECS/EntityManager.h"
#include "Common/common_macros.h"
#include "Core/ECS/JobSystem/TransformSystem.h"

Entity* create_entity(EntityManager* manager) {
	Entity* entity = &manager->entity_list[manager->last_entity_index];
	entity->entity_id = manager->last_entity_index;
	manager->last_entity_index++;
	return entity;
}

void add_component(EntityManager* manager, Entity* entity, ComponentType type) {

}

void init_entity_manager(EntityManager* manager, void* parition_start, size_t partition_size) {

	linear_init(&manager->mem, parition_start, partition_size);

	size_t entity_count = EntityManager::EntityCountList::size;
	
	// For now every entity has every component to test
	manager->comp_manager.transform_manager.count = entity_count;
	manager->comp_manager.transform_manager.positions = cast(Vec3f*) linear_alloc(&manager->mem, sizeof(Vec3f) * entity_count, 1);
	manager->comp_manager.transform_manager.scales = cast(Vec3f*) linear_alloc(&manager->mem, sizeof(Vec3f) * entity_count, 1);
	manager->comp_manager.transform_manager.rotations = cast(Quat*) linear_alloc(&manager->mem, sizeof(Quat) * entity_count, 1);
	manager->comp_manager.transform_manager.forwards = cast(Vec3f*) linear_alloc(&manager->mem, sizeof(Vec3f) * entity_count, 1);
	manager->comp_manager.transform_manager.ups = cast(Vec3f*) linear_alloc(&manager->mem, sizeof(Vec3f) * entity_count, 1);
	manager->comp_manager.transform_manager.rights = cast(Vec3f*) linear_alloc(&manager->mem, sizeof(Vec3f) * entity_count, 1);

	manager->comp_manager.transform_manager.local = cast(Mat4x4f*) linear_alloc(&manager->mem, sizeof(Mat4x4f) * entity_count, 1);
	manager->comp_manager.transform_manager.world = cast(Mat4x4f*) linear_alloc(&manager->mem, sizeof(Mat4x4f) * entity_count, 1);

	manager->comp_manager.transform_manager.parent = cast(int*) linear_alloc(&manager->mem, sizeof(int) * entity_count, 1);
	manager->comp_manager.transform_manager.first_child = cast(int*) linear_alloc(&manager->mem, sizeof(int) * entity_count, 1);
	manager->comp_manager.transform_manager.next_sibling = cast(int*) linear_alloc(&manager->mem, sizeof(int) * entity_count, 1);
	//manager->comp_manager.transform_manager.prev_sibling = cast(int*) linear_alloc(&manager->mem, sizeof(int) * entity_count, 1);
	


	manager->comp_manager.camera_manager.count = entity_count;
	manager->comp_manager.camera_manager.cameras = cast(Camera*) linear_alloc(&manager->mem, sizeof(Camera) * entity_count, 1);

	manager->comp_manager.static_mesh_manager.count = entity_count;
	manager->comp_manager.static_mesh_manager.meshes = cast(StaticMesh*) linear_alloc(&manager->mem, sizeof(StaticMesh) * entity_count, 1);


	job_default_transforms(&manager->comp_manager.transform_manager);

}

void attach_child_entity(EntityManager* manager, Entity entity, Entity child) {
	
	int* first_child = &manager->comp_manager.transform_manager.first_child[entity.entity_id];
	if (*first_child == NO_ENTITY_ID) {
		// This entity has no children, so we need to add this child to the first child
		*first_child = child.entity_id;
	} else {
		// This entity already has a first child so we need to look at the siblings
		int* next_sibling = &manager->comp_manager.transform_manager.next_sibling[*first_child];

		while (*next_sibling != NO_ENTITY_ID) {
			next_sibling = &manager->comp_manager.transform_manager.next_sibling[*next_sibling];
		}
		*next_sibling = child.entity_id;
		
	}
	manager->comp_manager.transform_manager.parent[child.entity_id] = entity.entity_id;
}

Camera* get_camera(EntityManager* manager, Entity entity) {
	return &manager->comp_manager.camera_manager.cameras[entity.entity_id];
}

StaticMesh* get_static_mesh(EntityManager* manager, Entity entity) {
	return &manager->comp_manager.static_mesh_manager.meshes[entity.entity_id];
}
Mat4x4f* get_world_mat(EntityManager* manager, Entity entity) {
	return &manager->comp_manager.transform_manager.world[entity.entity_id];
}

Mat4x4f* get_local_mat(EntityManager* manager, Entity entity) {
	return &manager->comp_manager.transform_manager.local[entity.entity_id];
}

Vec3f position(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.positions[entity.entity_id];
}

void set_position(EntityManager* manager, Entity entity, Vec3f pos) {
	manager->comp_manager.transform_manager.positions[entity.entity_id] = pos;
}

Vec3f get_scale(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.scales[entity.entity_id];
}

void set_scale(EntityManager* manager, Entity entity, Vec3f scale) {
	manager->comp_manager.transform_manager.scales[entity.entity_id] = scale;
}

Quat rotation(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.rotations[entity.entity_id];
}

void set_rotation(EntityManager* manager, Entity entity, Quat rotation) {
	manager->comp_manager.transform_manager.rotations[entity.entity_id] = rotation;
}

Vec3f forward(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.forwards[entity.entity_id];
}

Vec3f up(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.ups[entity.entity_id];
}

Vec3f right(EntityManager* manager, Entity entity) {
	return manager->comp_manager.transform_manager.rights[entity.entity_id];
}