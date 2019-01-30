#pragma once

#include "Core/ECS/JobSystem/TransformSystem.h"


void job_update_basis_vectors(EntityManager* manager) {
	Vec3f forward = Vec3f_Forward;
	Vec3f up = Vec3f_Up;
	Vec3f right = Vec3f_Right;

	TransformManager* tm = &manager->transform_manager;

	for (int i = 0; i < tm->count; i++) {
		tm->forwards[i] = tm->rotations[i] * forward;
	}
	for (int i = 0; i < tm->count; i++) {
		tm->ups[i] = tm->rotations[i] * up;
	}
	for (int i = 0; i < tm->count; i++) {
		tm->rights[i] = tm->rotations[i] * right;
	}

}




static void world_transform(TransformManager* tm, Mat4x4f* parent, Entity e) {

	MapResult<u64> result = map_get(&tm->id_map, e.id);
	assert(result.found);
	if (!result.found) {}
	int index = result.value;

	tm->world[index] = *parent * tm->local[index];
	Entity child = tm->first_child[index];
	while (child.id != NO_ENTITY_ID) {
		world_transform(tm, &tm->world[index], child);
		MapResult<u64> result = map_get(&tm->id_map, child.id);
		assert(result.found);
		if (!result.found) {}
		u64 child_index = result.value;
		child = tm->next_sibling[child_index];
	}
}

static Mat4x4f get_world_transform(EntityManager* manager, Entity e) {

	TransformManager* tm = &manager->transform_manager;
	
	
	u64 index = get_index_for_entity(manager, e, &manager->transform_manager.id_map);
	Entity parent_entity = tm->parent[index];
	// If this entity has no parent, then we'll just use the current local matrix
	if (parent_entity.id == NO_ENTITY_ID) {
		return tm->local[index];
	} else {
		return get_world_transform(manager, parent_entity)* *get_local_mat(manager, e);
	}
}



void job_compute_world_matrices(EntityManager* manager) {

	TransformManager* tm = &manager->transform_manager;


	// Scale first
	for (int i = 0; i < tm->count; i++) {
		tm->local[i] = scale(tm->scales[i]);
	}

	// Then rotate
	for (int i = 0; i < tm->count; i++) {
		// Build the rotation matrix from our quat
		Mat4x4f r = quat_to_rotation_matrix(tm->rotations[i]);
		tm->local[i] = r * tm->local[i];
	}

	// Then translate
	for (int i = 0; i < tm->count; i++) {
		Mat4x4f t = translate(tm->positions[i]);
		tm->local[i] = t * tm->local[i];
		
	}

	// Go through each entity's transform
	for (int i = 0; i < tm->count; i++) {
		int index = i;
		Entity e;
		e.id = index + 1;
		// TODO: this is very slow and cache unfriendly
		// A better solution woould be to sort the parent/dirty to the front of the array
		// http://bitsquid.blogspot.com/2014/10/building-data-oriented-entity-system.html
		
		tm->world[index] = get_world_transform(manager, e);
		
		
		
		//// Get the parent of the current entity
		//Entity parent_id = tm->parent[index];
		//// If this entity has no parent, then we'll just use the current local matrix
		//if (parent_id.id == NO_ENTITY_ID) {
		//	tm->world[index] = tm->local[index];
		//} else {	
		//
		//	// If we do have a parent
		//	// Then get the index into the world matrix of the parent entity
		//	MapResult<u64> result = map_get(&tm->id_map, parent_id.id);
		//	assert(result.found);
		//	u64 parent_index = result.value;
		
		//
		//	 
		// The current problem with this is that, the parent world matrix might not have been already computed,
		// so we need to compute it before we compute the world matrix of the child

		// Get parent world transform
		//	Mat4x4f parent_transform = tm->world[parent_index];
		//	Entity e;
		//	e.id = index;
		//	world_transform(tm, &parent_transform, e);
		//}
		
	}	
	
}

void set_name(EntityManager* manager, Entity entity, char* name) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	manager->transform_manager.names[index] = name;
}

const char* get_name(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.names[index];
}

Mat4x4f* get_world_mat(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return &manager->transform_manager.world[index];
}

Mat4x4f* get_local_mat(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return &manager->transform_manager.local[index];
}

Vec3f position(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.positions[index];
}

void set_position(EntityManager* manager, Entity entity, Vec3f pos) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	manager->transform_manager.positions[index] = pos;
}

Vec3f get_scale(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.scales[index];
}

void set_scale(EntityManager* manager, Entity entity, Vec3f scale) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	manager->transform_manager.scales[index] = scale;
}

Quat rotation(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.rotations[index];
}

void set_rotation(EntityManager* manager, Entity entity, Quat rotation) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	manager->transform_manager.rotations[index] = rotation;
}

Vec3f forward(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.forwards[index];
}

Vec3f up(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.ups[index];
}

Vec3f right(EntityManager* manager, Entity entity) {
	int index = get_index_for_entity(manager, entity, &manager->transform_manager.id_map);
	return manager->transform_manager.rights[index];
}

