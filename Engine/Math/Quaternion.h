#pragma once

#include "Math.h"
#include "Vec.h"
#include "Mat.h"

typedef struct Quaternion {
	float w;
	union {
		struct {
			float x;
			float y;
			float z;
		};
		Vec3f v;
	};
	
} Quaternion;


// Alias so that we dont have to type "Quaternion" everytime we want to use them
// Qaut is easier, faster, less verbose to type
typedef Quaternion Quat;


Quat inline quat_identity() {
	Quat result;
	result.w = 1.0f;
	result.v = Vec3f_Zero;
	return result;
}

// Where 
Quat inline quat_from_axis_angle(Vec3f n, float deg) {
	Quat result;
	// Convert to radians
	float rad = deg_to_rad(deg);
	float half_angle = rad * 0.5f;
	float sinf_half_angle = sinf_(half_angle);


	result.w = cosf_(half_angle);
	result.x = n.x * sinf_half_angle;
	result.y = n.y * sinf_half_angle;
	result.z = n.z * sinf_half_angle;

	return result;
}

Quat inline quat_invert(Quat q) {
	Quat result;
	result.w = q.w;

	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;

	return result;
}


Quat inline operator*(Quat q1, Quat q2) {
	Quat result;
	result.w = (q1.w * q2.w) - (dot(q1.v, q2.v));
	// Note q1 * q2 != q2 * q1
	result.v = ((q1.w * q2.v) + (q2.w * q1.v)) + cross(q1.v, q2.v);
	
	return result;
}

Vec3f inline operator*(Quat q, Vec3f pt) {
	Vec3f result;
	Quat pt_quat;
	pt_quat.w = 0;
	pt_quat.v = pt;

	// TODO: optimize
	result = ((q * pt_quat) * quat_invert(q)).v;

	return result;
}

// TODO: implement slerp

// TODO: quat to rotation matrix

Mat4x4f inline quat_to_rotation_matrix(Quat q) {
	Mat4x4f result = mat4x4f_identity();
	float qx2 = q.x * q.x;
	float qy2 = q.y * q.y;
	float qz2 = q.z * q.z;


	result.mat2d[0][0] = 1 - 2 * (qy2 + qz2);
	result.mat2d[0][1] = 2 * (q.x * q.y - q.z * q.w);
	result.mat2d[0][2] = 2 * (q.x * q.z + q.y * q.w);

	result.mat2d[1][0] = 2 * (q.x * q.y + q.z * q.w);
	result.mat2d[1][1] = 1 - 2 * (qx2 + qz2);
	result.mat2d[1][2] = 2 * (q.y * q.z - q.x * q.w);

	result.mat2d[2][0] = 2 * (q.x * q.z - q.y * q.w);
	result.mat2d[2][1] = 2 * (q.y * q.z + q.x * q.w);
	result.mat2d[2][2] = 1 - 2 * (qx2 + qy2);


	return result;
}