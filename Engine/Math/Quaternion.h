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

	Quaternion() : x(0), y(0), z(0), w(1) {};
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	
} Quaternion;


// Alias so that we dont have to type "Quaternion" everytime we want to use them
// Qaut is easier, faster, less verbose to type
typedef Quaternion Quat;



Quat quat_from_axis_angle(const Vec3f& n, float deg);
Quat quat_invert(const Quat& q);
Quat operator*(const Quat& q1, const Quat& q2);
Vec3f operator*(const Quat& q, const Vec3f& pt);
Mat4x4f quat_to_rotation_matrix(const Quat& q);

// TODO: implement slerp