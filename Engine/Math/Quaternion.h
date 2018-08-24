#pragma once

#include "Math.h"
#include "Vec.h"

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



Quat inline axis_angle_to_quat(Vec3f n, float a) {
	Quat result;
	// Convert to radians
	a = deg_to_rad(a);
	float half_a = a * 0.5f;
	result.w = cosf(half_a);

	float sinf_half_a = sinf(half_a);

	result.x = n.x * sinf_half_a;
	result.y = n.y * sinf_half_a;
	result.z = n.z * sinf_half_a;

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


Quat inline quat_mult(Quat q1, Quat q2) {
	Quat result;
	result.w = (q1.w * q2.w) - (v3_dot(q1.v, q2.v));
	// Note q1 * q2 != q2 * q1
	result.v = v3_add(v3_add(v3_multiply(q1.w, q2.v), v3_multiply(q2.w, q1.v)), v3_cross(q1.v, q2.v));
	
	return result;
}

Vec3f inline quat_mult_pt(Quat q, Vec3f pt) {
	Vec3f result;
	Quat pt_quat;
	pt_quat.w = 0;
	pt_quat.v = pt;

	// TODO: optimize
	result = quat_mult(quat_mult(q, pt_quat), quat_invert(q)).v;

	return result;
}

// TODO: implement slerp

// TODO: quat to rotation matrix