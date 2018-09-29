#pragma once

#include "Core/ECS/JobSystem/TransformSystem.h"


void job_update_basis_vectors(EntityManager* manager) {
	Vec3f forward = Vec3f_Forward;
	Vec3f up = Vec3f_Up;
	Vec3f right = Vec3f_Right;

	TransformManager* tm = &manager->transforms;

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





void job_compute_world_matrices(EntityManager* manager) {

	TransformManager* tm = &manager->transforms;


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
		t = transpose(t);
		tm->local[i] = t * tm->local[i];
	}


	for (int i = 0; i < tm->count; i++) {
		int index = i;
		Entity parent_id = tm->parent[index];
		if (parent_id.id == NO_ENTITY_ID) {
			tm->world[index] = tm->local[index];
		} else {
			
			uint64_t parent_index = map_get(&tm->id_map, parent_id.id);
			Mat4x4f parent_transform = tm->world[parent_index];
			

			for (;;) {// keep transforming until we have no more children
				tm->world[index] = parent_transform * tm->local[index];
				Entity child = tm->first_child[index];
				while (child.id != NO_ENTITY_ID) {
					
					uint64_t child_index = map_get(&tm->id_map, child.id);
					child = tm->next_sibling[child_index];
				}
				break;
			}
			
		}
		
	}	
	


}

Mat4x4f* get_world_mat(EntityManager* manager, Entity entity) {
	
	int index = map_get(&manager->transforms.id_map, entity.id);
	return &manager->transforms.world[index];
}

Mat4x4f* get_local_mat(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return &manager->transforms.local[index];
}

Vec3f position(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.positions[index];
}

void set_position(EntityManager* manager, Entity entity, Vec3f pos) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	manager->transforms.positions[index] = pos;
}

Vec3f get_scale(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.scales[index];
}

void set_scale(EntityManager* manager, Entity entity, Vec3f scale) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	manager->transforms.scales[index] = scale;
}

Quat rotation(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.rotations[index];
}

void set_rotation(EntityManager* manager, Entity entity, Quat rotation) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	manager->transforms.rotations[index] = rotation;
}

Vec3f forward(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.forwards[index];
}

Vec3f up(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.ups[index];
}

Vec3f right(EntityManager* manager, Entity entity) {
	int index = map_get(&manager->transforms.id_map, entity.id);
	return manager->transforms.rights[index];
}