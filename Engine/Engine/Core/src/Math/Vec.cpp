#pragma once

#include "Math/Vec.h"



// Vec 4 operations


// Addition
Vec4f operator+(const Vec4f& v1, const Vec4f& v2) {

#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(v1.data);
	__m128 m2 = _mm_load_ps(v2.data);
	__m128 mresult = _mm_add_ps(m1, m2);
	Vec4f result;
	_mm_store_ps(result.data, mresult);

	return result;
#else
	Vec4f result = {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
		v1.w + v2.w,
	};
	return result;
#endif

}

Vec4f& operator+=(Vec4f& v1, const Vec4f& v) {
	v1 = v1 + v;
	return v1;
}

// Equality
bool operator==(const Vec4f& v1, const Vec4f& v2) {
	return
		v1.x == v2.x &&
		v1.y == v2.y &&
		v1.z == v2.z &&
		v1.w == v2.w;
}

// Subtraction
Vec4f operator-(const Vec4f& v1, const Vec4f& v2) {


#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(v1.data);
	__m128 m2 = _mm_load_ps(v2.data);
	__m128 mresult = _mm_sub_ps(m1, m2);
	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result;
#else
	Vec4f result = {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
		v1.w - v2.w,
	};
	return result;
#endif
}

// Negation
Vec4f operator-(const Vec4f& vec) {

#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(vec.data);
	__m128 mresult = _mm_sub_ps(_mm_set1_ps(0.0), m1);
	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result;
#else
	Vec4f result = {
		-vec.x
		- vec.y
		- vec.z
		- vec.w
	};
	return result;
#endif
}

// Scalar multiplication
Vec4f operator*(float scalar, const Vec4f& vec) {

#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(vec.data);
	//__m128 m1 = _mm_setr_ps(vec.x, vec.y, vec.z, vec.w);
	__m128 mresult = _mm_mul_ps(_mm_set1_ps(scalar), m1);
	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result;
#else
	Vec4f result = {
		scalar * vec.x,
		scalar * vec.y,
		scalar * vec.z,
		scalar * vec.w,
	};
	return result;
#endif

}
// Scalar multiplication
Vec4f operator*(const Vec4f& vec, float scalar) {
	return scalar * vec;
}


float dot(const Vec4f& v1, const Vec4f& v2) {

#ifdef _INCLUDED_SMM 
	__m128 m1 = _mm_load_ps(v1.data);
	__m128 m2 = _mm_load_ps(v2.data);
	// 0xF1 == 0000 0000 1111 0001, meaning do the dot product on all 1111, and place the result at the bottom 0001
	__m128 mresult = _mm_dp_ps(m1, m2, 0xF1);
	float result = _mm_cvtss_f32(mresult);
	return result;
#else
	float result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	return result;
#endif

}


float magnitude(const Vec4f& vec) {


#ifdef _INCLUDED_SMM 
	// This saves an extra load from v4_dot since, we are dotting m1 with itself
	__m128 m1 = _mm_load_ps(vec.data);
	__m128 mresult = _mm_dp_ps(m1, m1, 0xF1);
	float result = _mm_cvtss_f32(_mm_sqrt_ss(mresult));
	return result;
#elif _INCLUDED_MM2
	float result = dot(vec, vec);
	__m128 mresult = _mm_sqrt_ss(_mm_set1_ps(result));
	result = _mm_cvtss_f32(mresult);
	return result;
#else
	float result = dot(vec, vec);
	return sqrtf_(result);
#endif
}

// NOTE: this is not as precise as it can be
// Since rsqrt is an approximate it is faster, but less precise. in the future if u need a more precise normalize
// create a funcition with sqrt
Vec4f normalize(const Vec4f& vec) {
#ifdef _INCLUDED_SMM 
	__m128 m1 = _mm_load_ps(vec.data);
	__m128 inverse_norm = _mm_rsqrt_ps(_mm_dp_ps(m1, m1, 0xFF));
	__m128 mresult = _mm_mul_ps(m1, inverse_norm);

	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result;
#else

	float len = length(vec);

	float inv_len = 1 / len;
	Vec4f result = {
		vec.x * inv_len,
		vec.y * inv_len,
		vec.z * inv_len,
		vec.w * inv_len,
	};
	return result;
#endif


}


// Vec3f operator overloads


// Addition
Vec3f operator+(const Vec3f& v1, const Vec3f& v2) {
	Vec4f v41 = { v1.x, v1.y, v1.z, 0 };
	Vec4f v42 = { v2.x, v2.y, v2.z, 0 };
	Vec3f result = (v41 + v42).xyz;
	return result;
}

Vec3f& operator+=(Vec3f& v1, const Vec3f& v) {
	v1 = v1 + v;
	return v1;
}

// equality
bool operator==(const Vec3f& v1, const Vec3f& v2) {
	return
		v1.x == v2.x &&
		v1.y == v2.y &&
		v1.z == v2.z;
}

// Subtraction
Vec3f operator-(const Vec3f& v1, const Vec3f& v2) {
	Vec4f v41 = { v1.x, v1.y, v1.z, 0 };
	Vec4f v42 = { v2.x, v2.y, v2.z, 0 };
	Vec3f result = (v41 - v42).xyz;
	return result;
}

// Negation
Vec3f operator-(const Vec3f& v) {
	Vec4f v4 = { v.x, v.y, v.z, 0 };
	Vec3f result = (-v4).xyz;
	return result;
}

// Scalar multiplication
Vec3f operator*(float scalar, const Vec3f& v) {
	Vec4f v4 = { v.x, v.y, v.z, 0 };
	Vec3f result = (scalar * v4).xyz;
	return result;
}

// Scalar multiplication
Vec3f operator*(const Vec3f& v, float scalar) {
	return scalar * v;
}

float magnitude(const Vec3f& vec) {
	float result = magnitude(Vec4f(vec, 0));
	return result;
}

Vec3f normalize(const Vec3f& vec) {
	Vec3f result = normalize(Vec4f(vec, 0)).xyz;
	return result;
}

float dot(const Vec3f& v1, const Vec3f& v2) {
	float result = dot(Vec4f(v1, 0), Vec4f(v2, 0));
	return result;
}

Vec3f cross(const Vec3f& v1, const Vec3f& v2) {

#ifdef _INCLUDED_MM2
	Vec4f v41 = Vec4f(v1, 0);
	Vec4f v42 = Vec4f(v2, 0);

	__m128 m1 = _mm_load_ps(v41.data);
	__m128 m2 = _mm_load_ps(v42.data);
	// Shuffle
	//_MM_SHUFFLE(param1, param2, param3, param4 )
	//R[0] = m1[param4]
	//R[1] = m1[param3]
	//R[2] = m2[param2]
	//R[3] = m2[param1]
	__m128 mresult = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(m1, m1, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(m2, m2, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(_mm_shuffle_ps(m1, m1, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(m2, m2, _MM_SHUFFLE(3, 0, 2, 1))));
	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result.xyz;
#else
	Vec3f result;
	result.data[0] = v1.data[1] * v2.data[2] - v1.data[2] * v2.data[1];
	result.data[1] = v1.data[2] * v2.data[0] - v1.data[0] * v2.data[2];
	result.data[2] = v1.data[0] * v2.data[1] - v1.data[1] * v2.data[0];
	return result;
#endif

}




Vec3f angle_axis_rotate(const Vec3f& v, const Vec3f& n, float rads) {
	/*Vec3f result = v3_add(
	v3_add(v3_multiply(cosf_(rads), v), v3_multiply(sinf_(rads), v3_cross(n, v))),
	v3_multiply((1 - cosf_(rads)) * v3_dot(n, v), n));*/
	Vec3f result = (cosf_(rads) * v) + (sinf_(rads) * cross(n, v)) + (((1 - cosf_(rads)) * dot(n, v)) * n);
	return result;

}

Vec2i operator-(const Vec2i& v1, const Vec2i& v2) { // Subtraction 
	Vec2i result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	return result;
}

float magnitude(const Vec2i& vec) {
	float result;
	result = sqrtf_((vec.x * vec.x) + (vec.y * vec.y));
	return result;
}


Vec3f euler_to_vector(const Vec3f& euler_angles) {
	Vec3f result;
	// Since our x,z axis are flipped, we also flip the calculation
	result.x = sinf_(DEG2RAD(euler_angles.y)) * cosf_(DEG2RAD(euler_angles.x));
	result.y = sinf_(DEG2RAD(euler_angles.x));
	result.z = cosf_(DEG2RAD(euler_angles.y)) * cosf_(DEG2RAD(euler_angles.x));
	return result;
}

Vec3f swap_yz(const Vec3f& v) {
	Vec3f result;
	float temp = v.y;
	result.y = v.z;
	result.z = temp;
	return result;
}

Vec3f swap_xy(const Vec3f& v) {
	Vec3f result;
	float temp = v.x;
	result.x = v.y;
	result.y = temp;
	return result;
}

Vec3f swap_xz(const Vec3f& v) {
	Vec3f result;
	float temp = v.x;
	result.x = v.z;
	result.z = temp;
	return result;
}