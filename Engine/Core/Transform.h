#pragma once

#include "../Math/Vec.h"
#include "../Math/Mat.h"
#include "../Math/Quaternion.h"


typedef struct Transform {
	Vec3f position;
	Vec3f scale;
	Vec3f euler_angles;

	Vec3f forward;
	Vec3f up;
	Vec3f right;

	Quat rotation;

} Transform;


void inline init_transform(Transform* transform) {
	transform->position = Vec3f_Zero;
	transform->scale = Vec3f_One;
	transform->euler_angles = Vec3f_Zero;

	transform->forward = Vec3f_Forward;
	transform->up = Vec3f_Up;
	transform->right = Vec3f_Right;

	transform->rotation = quat_identity();

}


Mat4x4f inline scale(Vec3f vec) {
	Mat4x4f mat = mat4x4f_identity();
	mat.m00 = vec.x;
	mat.m11 = vec.y;
	mat.m22 = vec.z;
	return mat;
}



// TODO: figure out why we need to transpose everything
// I think some thinks are column major, and some things are row major
Mat4x4f inline translate(Vec3f vec) {
	Mat4x4f mat = mat4x4f_identity();
	mat.m03 = vec.x;
	mat.m13 = vec.y;
	mat.m23 = vec.z;
	return mat;
}

Mat4x4f inline rotate_with_mat(Mat4x4f m, float rads, Vec3f axis) {


	float cos_angle = cosf(rads);
	float sin_angle = sinf(rads);

	float one_minus_cos = 1 - cos_angle;

	float xy = axis.x * axis.y;
	float yz = axis.y * axis.z;
	float xz = axis.x * axis.z;

	m.mat2d[0][0] = axis.x * axis.x * one_minus_cos + cos_angle;
	m.mat2d[0][1] = xy * one_minus_cos - axis.z * sin_angle;
	m.mat2d[0][2] = xz * one_minus_cos + axis.y * sin_angle;

	m.mat2d[1][0] = xy * one_minus_cos + axis.z * sin_angle;
	m.mat2d[1][1] = axis.y * axis.y * one_minus_cos + cos_angle;
	m.mat2d[1][2] = yz * one_minus_cos - axis.x * sin_angle;

	m.mat2d[2][0] = xz * one_minus_cos - axis.y * sin_angle;
	m.mat2d[2][1] = yz * one_minus_cos + axis.x * sin_angle;
	m.mat2d[2][2] = axis.z * axis.z * one_minus_cos + cos_angle;

	//rot = transpose(&rot);
	return m;
}

Mat4x4f inline rotate(float rads, Vec3f axis) {
	Mat4x4f rot = mat4x4f_identity();
	return rotate_with_mat(rot, rads, axis);
}


Mat4x4f inline trs_mat_from_transform(Transform* transform) {
	Mat4x4f result = mat4x4f_identity();

	// Scale matrix
	Mat4x4f s = scale(transform->scale);


	// Build quat from our euler angles
	Quat q;
	q =				 quat_from_axis_angle(Vec3f_Up, transform->euler_angles.y);
	q = quat_mult(q, quat_from_axis_angle(Vec3f_Right, transform->euler_angles.x));
	q = quat_mult(q, quat_from_axis_angle(Vec3f_Forward, transform->euler_angles.z));

	// Build the rotation matrix from our quat
	Mat4x4f r = quat_to_rotation_matrix(q);

	// Build the translate matrix
	Mat4x4f t = translate(transform->position);
	t = transpose(&t);


	// Scale first
	// Then rotate
	// Then translate
	result = mat4x4_mul(&r, &s);
	result = mat4x4_mul(&t, &result);
	


	return result;

}