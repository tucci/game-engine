#pragma once

#include "Core/ECS/JobSystem/TransformSystem.h"

void job_default_transforms(TransformManager* manager) {
	Vec3f zero = Vec3f_Zero;
	Vec3f one = Vec3f_One;
	Vec3f forward = Vec3f_Forward;
	Vec3f up = Vec3f_Up;
	Vec3f right = Vec3f_Right;
	Quat identity = Quat();

	for (int i = 0; i < manager->count; i++) {
		manager->positions[i] = zero;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->scales[i] = one;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->forwards[i] = forward;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->ups[i] = up;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->rights[i] = right;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->rotations[i] = identity;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->parent[i] = NO_ENTITY_ID;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->first_child[i] = NO_ENTITY_ID;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->next_sibling[i] = NO_ENTITY_ID;
	}

	for (int i = 0; i < manager->count; i++) {
		//manager->prev_sibling[i] = NO_ENTITY_ID;
	}

	for (int i = 0; i < manager->count; i++) {
		manager->local[i] = Mat4x4f();
	}

	for (int i = 0; i < manager->count; i++) {
		manager->world[i] = Mat4x4f();
	}
}

void job_update_basis_vectors(TransformManager* manager) {
	Vec3f forward = Vec3f_Forward;
	Vec3f up = Vec3f_Up;
	Vec3f right = Vec3f_Right;

	for (int i = 0; i < manager->count; i++) {
		manager->forwards[i] = manager->rotations[i] * forward;
	}
	for (int i = 0; i < manager->count; i++) {
		manager->ups[i] = manager->rotations[i] * up;
	}
	for (int i = 0; i < manager->count; i++) {
		manager->rights[i] = manager->rotations[i] * right;
	}

}





void job_compute_world_matrices(TransformManager* manager) {

	// Scale first
	for (int i = 0; i < manager->count; i++) {
		manager->local[i] = scale(manager->scales[i]);
	}

	// Then rotate
	for (int i = 0; i < manager->count; i++) {
		// Build the rotation matrix from our quat
		Mat4x4f r = quat_to_rotation_matrix(manager->rotations[i]);
		manager->local[i] = r * manager->local[i];
	}

	// Then translate
	for (int i = 0; i < manager->count; i++) {
		
		Mat4x4f t = translate(manager->positions[i]);
		t = transpose(t);
		manager->local[i] = t * manager->local[i];
	}


	for (int i = 0; i < manager->count; i++) {

		int parent_id = manager->parent[i];
		if (parent_id == NO_ENTITY_ID) {
			manager->world[i] = manager->local[i];
		} else {
			Mat4x4f parent_transform = manager->world[parent_id];
			

			for (;;) {// keep transforming until we have no more children
				manager->world[i] = parent_transform * manager->local[i];
				int child = manager->first_child[i];
				while (child != NO_ENTITY_ID) {
					child = manager->next_sibling[child];
				}
				break;
			}
			
		}
		
	}	
	


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