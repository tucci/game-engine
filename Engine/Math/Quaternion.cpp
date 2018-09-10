#pragma once

#include "Quaternion.h"

Quat quat_from_axis_angle(const Vec3f& n, float deg) {
	Quat result;
	// Convert to radians
	float rad = DEG2RAD(deg);
	float half_angle = rad * 0.5f;
	float sinf_half_angle = sinf_(half_angle);


	result.w = cosf_(half_angle);
	result.x = n.x * sinf_half_angle;
	result.y = n.y * sinf_half_angle;
	result.z = n.z * sinf_half_angle;

	return result;
}

Quat quat_invert(const Quat& q) {
	Quat result;
	result.w = q.w;

	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;

	return result;
}


Quat operator*(const Quat& q1, const Quat& q2) {
	Quat result;
	result.w = (q1.w * q2.w) - (dot(q1.v, q2.v));
	// Note q1 * q2 != q2 * q1
	result.v = ((q1.w * q2.v) + (q2.w * q1.v)) + cross(q1.v, q2.v);
	//result = quat_normalize(result);
	return result;
}

Quat& operator*=(Quat& q1, const Quat& q2) {
	q1 = q1 * q2;
	return q1;
}

Quat quat_normalize(const Quat& q) {
	Quat result = q;
	float length = magnitude(Vec4f(q.x, q.y, q.z, q.w));
	result.x /= length;
	result.y /= length;
	result.z /= length;
	result.w /= length;
	return result;
}
Vec3f operator*(const Quat& q, const Vec3f& pt) {
	Vec3f result;
	Quat pt_quat;
	pt_quat.w = 0;
	pt_quat.v = pt;

	// TODO: optimize simd
	result = ((q * pt_quat) * quat_invert(q)).v;

	return result;
}

// TODO: implement slerp


Mat4x4f quat_to_rotation_matrix(const Quat& q) {
	Mat4x4f result;
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


Vec3f quat_to_euler(const Quat& q) {

	//// Taken from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_Angles_Conversion
	Vec3f euler;

	//// roll (x-axis rotation)
	//float sinr = 2.0 * (q.w * q.x + q.y * q.z);
	//float cosr = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	//euler.x = atan2f_(sinr, cosr);

	//// pitch (y-axis rotation)
	//double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	//if (ABS(sinp) >= 1)
	//	euler.y = copysign_(PI / 2, sinp); // use 90 degrees if out of range
	//else
	//	euler.y = asinf_(sinp);

	//// yaw (z-axis rotation)
	//double siny = 2.0 * (q.w * q.z + q.x * q.y);
	//double cosy = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	//euler.z = atan2f_(siny, cosy);



	euler.x = atan2f_(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z);
	euler.y = atan2f_(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * q.y * q.y - 2 * q.z * q.z);
	euler.z = asinf_(2 * q.x * q.y + 2 * q.z * q.w);
	



	return euler;
}