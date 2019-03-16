#pragma once

#include "Math/Math.h"
#include "Math/Vec.h"
#include "Math/Mat.h"

struct Quaternion {
	
	union {
		struct {
			float x;
			float y;
			float z;
		};
		Vec3f v;
	};

	float w;

	float data[4];

	Quaternion() : x(0), y(0), z(0), w(1) {};
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	
};


// Alias so that we dont have to type "Quaternion" everytime we want to use them
// Qaut is easier, faster, less verbose to type
typedef Quaternion Quat;



Quat quat_from_axis_angle(const Vec3f& n, float deg);
Quat quat_conj(const Quat& q);
Quat quat_normalize(const Quat& q);
// Rotate around a local axis: rotation = rotation * quat_from_axis_angle(Vec3f_Up, angle)
// Rotate around a world axis : rotation = quat_from_axis_angle(Vec3f_Up, angle) * rotation;
Quat operator*(const Quat& q1, const Quat& q2);
Quat& operator*=(Quat& q1, const Quat& q2);
Vec3f operator*(const Quat& q, const Vec3f& pt);
Mat4x4f quat_to_rotation_matrix(const Quat& q);
Vec3f quat_to_euler(const Quat& q);
Quat euler_to_quat(const Vec3f& euler);

// TODO: implement slerp