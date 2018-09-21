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

void job_compute_trs_matrices(TransformManager* manager) {

	// Scale first
	for (int i = 0; i < manager->count; i++) {
		manager->model_mats[i] = scale(manager->scales[i]);
	}

	// Then rotate
	for (int i = 0; i < manager->count; i++) {
		// Build the rotation matrix from our quat
		Mat4x4f r = quat_to_rotation_matrix(manager->rotations[i]);
		manager->model_mats[i] = r * manager->model_mats[i];
	}

	// Then translate
	for (int i = 0; i < manager->count; i++) {
		
		Mat4x4f t = translate(manager->positions[i]);
		t = transpose(t);
		manager->model_mats[i] = t * manager->model_mats[i];
	}
}