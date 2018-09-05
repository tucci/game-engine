#pragma once



#include <intrin.h>
#include <xmmintrin.h>

#include "Math.h"


// TODO: make default constructors for our vecs
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
		union {
			struct {
				float x;
				float y;
			};
			struct {
				Vec2f xy;
			};
		};
		float z;

	};

	struct {
		float r;
		float g;
		float b;
	};

	struct {
		union {
			struct {
				float u;
				float v;
			};
			struct {
				Vec2f uv;
			};

		};
		float w;

	};



	float data[3];

} Vec3f;


typedef union Vec3i {

	struct {
		union {
			struct {
				int x;
				int y;
			};
			struct {
				Vec2i xy;
			};
		};
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

	

	int data[3];

	

} Vec3i;

typedef union Vec4f {
	struct {
		union {
			struct {
				float x;
				float y;
				float z;
			};

			struct {
				Vec3f xyz;
			};
		};
		float w;
	};

	struct {
		union {
			struct {
				float r;
				float g;
				float b;
			};

			struct {
				Vec3f rgb;
			};
		};
		float a;
	};
	

	float data[4];

} Vec4f;


typedef union Vec4i {

	struct {
		union {
			struct {
				int x;
				int y;
				int z;
			};

			struct {
				Vec3i xyz;
			};
		};
		int w;
	};

	struct {
		union {
			struct {
				int r;
				int g;
				int b;
			};

			struct {
				Vec3i rgb;
			};
		};
		int a;
	};

	int data[4];

} Vec4i;


#define Vec3f_Zero make_vec3f(0,0,0)
#define Vec3f_One make_vec3f(1,1,1)
#define Vec3f_Up make_vec3f(0,1,0)
#define Vec3f_Down make_vec3f(0,-1,0)
#define Vec3f_Forward make_vec3f(0,0,1)
#define Vec3f_Backward make_vec3f(0,0,-1)
#define Vec3f_Right make_vec3f(1,0,0)
#define Vec3f_Left make_vec3f(-1,0,0)

#define Vec4f_Zero make_vec4f(0,0,0,0)
#define Vec4f_One make_vec4f(1,1,1,1)



// Makes passing to vec parameters faster
#define vec34pack(v1, v2) vec3f_to_vec4f(v1, 0), vec3f_to_vec4f(v2, 0)





Vec2i inline make_vec2i(int x, int y) {
	Vec2i result = {
		x,
		y,
	};
	return result;
}

Vec2f inline make_vec2f(float x, float y) {
	Vec2f result = {
		x,
		y,
	};
	return result;
}


Vec3f inline make_vec3f(float x, float y, float z) {
	Vec3f result = {
		x,
		y,
		z
	};
	return result;
}

Vec3i inline make_vec3i(int x, int y, int z) {
	Vec3i result = {
		x,
		y,
		z
	};
	return result;
}

Vec4f inline make_vec4f(float x, float y, float z, float w) {
	Vec4f result = {
		x,
		y,
		z,
		w
	};
	return result;
}

Vec4f inline vec3f_to_vec4f(Vec3f v, float w) {
	Vec4f result = {
		v.x,
		v.y,
		v.z,
		w
	};
	return result;
}

Vec4i inline vec4f_to_vec4i(Vec4f v) {
	Vec4i result = {
		(int)v.x,
		(int)v.y,
		(int)v.z,
		(int)v.w,
	};
	return result;
}



// Vec 4 operations

// TODO: speed up with simd
Vec4f inline operator+(Vec4f v1, Vec4f v2) {

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

Vec4f inline operator-(Vec4f vec) {
	
#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(vec.data);
	__m128 mresult = _mm_sub_ps(_mm_set1_ps(0.0), m1);
	Vec4f result;
	_mm_store_ps(result.data, mresult);
	return result;
#else
	Vec4f result = {
		-vec.x
		-vec.y
		-vec.z
		-vec.w
	};
	return result;
#endif
}

Vec4f inline operator-(Vec4f v1, Vec4f v2) {


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

Vec4f inline operator*(float scalar, Vec4f vec) {

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


float inline dot(Vec4f v1, Vec4f v2) {

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


float inline magnitude(Vec4f vec) {
	

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
Vec4f inline normalize(Vec4f vec) {

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



Vec3f inline operator+(Vec3f v1, Vec3f v2) {
	Vec4f v41 = { v1.x, v1.y, v1.z, 0 };
	Vec4f v42 = { v2.x, v2.y, v2.z, 0 };
	Vec3f result = (v41 + v42).xyz;
	return result;
}

Vec3f inline operator-(Vec3f v) {
	Vec4f v4 = { v.x, v.y, v.z, 0 };
	Vec3f result = (-v4).xyz;
	return result;
}

Vec3f inline operator-(Vec3f v1, Vec3f v2) {
	Vec4f v41 = { v1.x, v1.y, v1.z, 0 };
	Vec4f v42 = { v2.x, v2.y, v2.z, 0 };
	Vec3f result = (v41 - v42).xyz;
	return result;
}

Vec3f inline operator*(float scalar, Vec3f v) {
	Vec4f v4 = { v.x, v.y, v.z, 0 };
	Vec3f result = (scalar * v4).xyz;
	return result;
}


float inline magnitude(Vec3f vec) {
	float result = magnitude(vec3f_to_vec4f(vec, 0));
	return result;
}

Vec3f inline normalize(Vec3f vec) {
	Vec3f result = normalize(vec3f_to_vec4f(vec, 0)).xyz;
	return result;
}

float inline dot(Vec3f v1, Vec3f v2) {
	float result = dot(vec34pack(v1, v2));
	return result;
}

Vec3f inline cross(Vec3f v1, Vec3f v2) {

#ifdef _INCLUDED_MM2
	Vec4f v41 = vec3f_to_vec4f(v1, 0);
	Vec4f v42 = vec3f_to_vec4f(v2, 0);

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



Vec3f inline angle_axis_rotate(Vec3f v, Vec3f n, float rads) {
	/*Vec3f result = v3_add(
		v3_add(v3_multiply(cosf_(rads), v), v3_multiply(sinf_(rads), v3_cross(n, v))),
		v3_multiply((1 - cosf_(rads)) * v3_dot(n, v), n));*/
	Vec3f result = (cosf_(rads) * v) + (sinf_(rads) * cross(n, v)) + (((1 - cosf_(rads)) * dot(n, v)) * n);
	return result;
	
}


Vec3f inline euler_to_vector(Vec3f euler_angles) {
	Vec3f result;
	// Since our x,z axis are flipped, we also flip the calculation
	result.x = sinf_(deg_to_rad(euler_angles.y)) * cosf_(deg_to_rad(euler_angles.x));
	result.y = sinf_(deg_to_rad(euler_angles.x));
	result.z = cosf_(deg_to_rad(euler_angles.y)) * cosf_(deg_to_rad(euler_angles.x));
	return result;
}

