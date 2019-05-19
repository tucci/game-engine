#pragma once

#include "Math/Quaternion.h"

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

Quat quat_conj(const Quat& q) {
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
Vec3f operator*(const Quat& q, const Vec3f& p) {
	Vec3f result;
	//Quat pt_quat;
	//pt_quat.w = 0;
	//pt_quat.v = v;

	// TODO: optimize simd
	//result = ((q * pt_quat) * quat_conj(q)).v;

	
	// Taken from https://www.essentialmath.com/GDC2012/GDC2012_JMV_Rotations.pdf
	Vec3f v(q.x, q.y, q.z);
	float w = q.w;
	// dot(v,v) because v is not normalized = 1 - (w^2)
	result =  ((w*w - dot(v, v)) * p) + (2.0f * dot(v, p) * v) + (2.0f * w * cross(v, p));
	return result;
}


Mat4x4f quat_to_rotation_matrix(const Quat& q) {
	Mat4x4f result;
	float qx2 = q.x * q.x;
	float qy2 = q.y * q.y;
	float qz2 = q.z * q.z;


	result.mat2d[0][0] = 1 - (2 * qy2) - (2 * qz2);
	result.mat2d[0][1] = (2 * q.x * q.y) - (2 * q.z * q.w);
	result.mat2d[0][2] = (2 * q.x * q.z) + (2 * q.y * q.w);

	result.mat2d[1][0] = (2 * q.x * q.y) + (2 * q.z * q.w);
	result.mat2d[1][1] = 1 - (2 * qx2) - (2 * qz2);
	result.mat2d[1][2] = (2 * q.y * q.z) - (2 * q.x * q.w);

	result.mat2d[2][0] = (2 * q.x * q.z) - (2 * q.y * q.w);
	result.mat2d[2][1] = (2 * q.y * q.z) + (2 * q.x * q.w);
	result.mat2d[2][2] = 1 - (2 * qx2) - (2 * qy2);


	result = transpose(result);
	return result;
}


Vec3f quat_to_euler(const Quat& q) {

	// Taken from https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/	
	double heading;
	double attitude;
	double bank;
	
	double sqw = q.w * q.w;
	double sqx = q.x * q.x;
	double sqy = q.y * q.y;
	double sqz = q.z * q.z;
	double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	double test = q.x * q.y + q.z * q.w;
	
	if (test > 0.499*unit) { // singularity at north pole
		heading = 2 * atan2(q.x, q.w);
		attitude = PI / 2;
		bank = 0;
		Vec3f euler(RAD2DEG(bank), RAD2DEG(heading), RAD2DEG(attitude));
		return euler;
	}
	if (test < -0.499*unit) { // singularity at south pole
		heading = -2 * atan2(q.x, q.w);
		attitude = -PI / 2;
		bank = 0;
		Vec3f euler(RAD2DEG(bank), RAD2DEG(heading), RAD2DEG(attitude));
		return euler;
	}
	heading = atan2(2 * q.y * q.w - 2 * q.x * q.z, sqx - sqy - sqz + sqw);
	attitude = asin(2 * test / unit);
	bank = atan2(2 * q.x * q.w - 2 * q.y * q.z, -sqx + sqy - sqz + sqw);
	
	
	
	Vec3f euler(RAD2DEG(bank), RAD2DEG(heading), RAD2DEG(attitude));
	return euler;


}

// Euler angles in degrees
Quat euler_to_quat(const Vec3f& euler) {
	
	

	//float pitch = DEG2RAD(euler.z);
	//float yaw = DEG2RAD(euler.x);
	//float roll = DEG2RAD(euler.y);
	
	
	Quat qx = quat_from_axis_angle(Vec3f_Right, euler.x);
	Quat qy = quat_from_axis_angle(Vec3f_Up, euler.y);
	Quat qz = quat_from_axis_angle(Vec3f_Forward, euler.z);
	
	// x y z backwards
	return qz * qy * qx;


	
}