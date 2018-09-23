#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"
#include "Math/Quaternion.h"

#define NO_ENTITY_ID -1


typedef struct Transform {
	

	Vec3f position;
	Vec3f scale;
	Quat rotation;

	
	// These may or may not be linked to the rotation. depends on the user
	Vec3f forward;
	Vec3f up;
	Vec3f right;

	

	Transform() {
		this->position = Vec3f_Zero;
		this->scale = Vec3f_One;
		this->forward = Vec3f_Forward;
		this->up = Vec3f_Up;
		this->right = Vec3f_Right;
		this->rotation = Quat();
	}
} Transform;

void inline update_direction_vectors_for_transform(Transform& transform) {
	transform.forward = transform.rotation * Vec3f_Forward;
	transform.up = transform.rotation * Vec3f_Up;
	transform.right = transform.rotation * Vec3f_Right;
}


Mat4x4f inline scale(const Vec3f& vec) {
	Mat4x4f mat;
	mat.m00 = vec.x;
	mat.m11 = vec.y;
	mat.m22 = vec.z;
	return mat;
}



// TODO: figure out why we need to transpose everything
// I think some thinks are column major, and some things are row major
Mat4x4f inline translate(const Vec3f& vec) {
	Mat4x4f mat;
	mat.m03 = vec.x;
	mat.m13 = vec.y;
	mat.m23 = vec.z;
	return mat;
}

Mat4x4f inline rotate(float rads, const Vec3f& axis) {
	Mat4x4f result;

	float cos_angle = cosf_(rads);
	float sin_angle = sinf_(rads);

	float one_minus_cos = 1 - cos_angle;

	float xy = axis.x * axis.y;
	float yz = axis.y * axis.z;
	float xz = axis.x * axis.z;

	result.mat2d[0][0] = axis.x * axis.x * one_minus_cos + cos_angle;
	result.mat2d[0][1] = xy * one_minus_cos - axis.z * sin_angle;
	result.mat2d[0][2] = xz * one_minus_cos + axis.y * sin_angle;

	result.mat2d[1][0] = xy * one_minus_cos + axis.z * sin_angle;
	result.mat2d[1][1] = axis.y * axis.y * one_minus_cos + cos_angle;
	result.mat2d[1][2] = yz * one_minus_cos - axis.x * sin_angle;

	result.mat2d[2][0] = xz * one_minus_cos - axis.y * sin_angle;
	result.mat2d[2][1] = yz * one_minus_cos + axis.x * sin_angle;
	result.mat2d[2][2] = axis.z * axis.z * one_minus_cos + cos_angle;


	return result;
}


Mat4x4f inline trs_mat_from_transform(Transform* transform) {
	Mat4x4f result;

	// Scale matrix
	Mat4x4f s = scale(transform->scale);


	// Build quat from our euler angles
	// Build the rotation matrix from our quat
	Quat q = transform->rotation;
	Mat4x4f r = quat_to_rotation_matrix(q);

	// Build the translate matrix
	Mat4x4f t = translate(transform->position);
	t = transpose(t);


	// Scale first
	// Then rotate
	// Then translate
	result = r * s;
	result = t * result;
	


	return result;

}


typedef struct TransformManager {
	int count;

	Vec3f* positions;
	Vec3f* scales;
	Vec3f* forwards;
	Vec3f* ups;
	Vec3f* rights;
	Quat* rotations;

	Mat4x4f* local;
	Mat4x4f* world;

	int* parent;
	int* first_child;
	int* next_sibling;
	//int* prev_sibling;

	
};

