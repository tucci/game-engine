#pragma once

typedef union Vec2i {
	struct {
		int x;
		int y;
	};
	float data[2];

} Vec2i;

typedef union Vec2f {
	struct {

		float x;
		float y;
	};
	struct {
		float u;
		float v;
	};
	float data[2];

} Vec2f;

typedef union Vec3f {

	struct {

		float x;
		float y;
		float z;
	};

	struct {
		float r;
		float g;
		float b;
	};

	struct {
		float u;
		float v;
		float w;
	};

	struct {
		Vec2f xy_;
		float __z;
	};

	float data[3];

} Vec3f;


typedef union Vec3i {

	struct {

		int x;
		int y;
		int z;
	};

	struct {
		int r;
		int g;
		int b;
	};

	struct {
		int u;
		int v;
		int w;
	};

	struct {
		Vec2i xy_;
		int __z;
	};

	int data[3];

} Vec3i;

typedef union Vec4f {

	struct {

		float x;
		float y;
		float z;
		float w;
	};

	struct {
		float r;
		float g;
		float b;
		float a;
	};

	struct {
		Vec3f xyz_;
		float ___w; // TODO: double underscore is reserved for specs. need to change this
	};

	float data[4];

} Vec4f;

#define Vec3f_Zero (Vec3f) {0,0,0}
#define Vec3f_One (Vec3f) {1,1,1}
#define Vec3f_Up (Vec3f) {0,1,0}
#define Vec3f_Down (Vec3f) {0,-1,0}
#define Vec3f_Forward (Vec3f) {0,0,1}
#define Vec3f_Backward (Vec3f) {0,0,-1}
#define Vec3f_Right (Vec3f) {1,0,0}
#define Vec3f_Left (Vec3f) {-1,0,0}

#define Vec4f_Zero (Vec4f) {0,0,0,0}
#define Vec4f_One (Vec4f) {1,1,1,1}




// TODO: speed up with simd

Vec3f inline vec_add(Vec3f v1, Vec3f v2) {
	Vec3f result = {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
	};
	return result;
}

Vec3f inline vec_negate(Vec3f vec) {
	Vec3f result = {
		-vec.x,
		-vec.y,
		-vec.z,
	};
	return result;
}

Vec3f inline vec_sub(Vec3f v1, Vec3f v2) {
	Vec3f result = {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
	};
	return result;
}

Vec3f inline vec_multiply(float scalar, Vec3f v) {
	Vec3f result = {
		scalar * v.x,
		scalar * v.y,
		scalar * v.z,
	};
	return result;
}



float inline vec_length(Vec3f vec) {
	// Replace sqrt with simd ones
	float length = sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	return length;
}

Vec3f inline vec_normalize(Vec3f vec) {
	float len = vec_length(vec);
	float inv_len = 1 / len;
	Vec3f result = {
		vec.x * inv_len,
		vec.y * inv_len,
		vec.z * inv_len,
	};
	return result;
}

float inline vec_dot(Vec3f v1, Vec3f v2) {
	float dot = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return dot;
}

Vec3f inline vec_cross(Vec3f v1, Vec3f v2) {
	Vec3f cross;
	cross.data[0] = v1.data[1] * v2.data[2] - v1.data[2] * v2.data[1];
	cross.data[1] = v1.data[2] * v2.data[0] - v1.data[0] * v2.data[2];
	cross.data[2] = v1.data[0] * v2.data[1] - v1.data[1] * v2.data[0];
	return cross;
}

