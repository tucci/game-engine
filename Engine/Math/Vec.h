#pragma once

//#include <math.h>

#include <intrin.h>
#include <xmmintrin.h>



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



// Makes passing to vec parameters faster
#define vec34pack(v1, v2) vec3f_to_vec4f(v1, 0), vec3f_to_vec4f(v2, 0)


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
Vec4f inline v4_add(Vec4f v1, Vec4f v2) {

#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(&v1);
	__m128 m2 = _mm_load_ps(&v2);
	__m128 mresult = _mm_add_ps(m1, m2);
	Vec4f result;
	_mm_store_ps(&result, mresult);
	
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

Vec4f inline v4_negate(Vec4f vec) {
	
#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(&vec);
	__m128 mresult = _mm_sub_ps(_mm_set1_ps(0.0), m1);
	Vec4f result;
	_mm_store_ps(&result, mresult);
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

Vec4f inline v4_sub(Vec4f v1, Vec4f v2) {


#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(&v1);
	__m128 m2 = _mm_load_ps(&v2);
	__m128 mresult = _mm_sub_ps(m1, m2);
	Vec4f result;
	_mm_store_ps(&result, mresult);
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

Vec4f inline v4_multiply(float scalar, Vec4f vec) {

#ifdef _INCLUDED_MM2
	__m128 m1 = _mm_load_ps(&vec);
	//__m128 m1 = _mm_setr_ps(vec.x, vec.y, vec.z, vec.w);
	__m128 mresult = _mm_mul_ps(_mm_set1_ps(scalar), m1);
	Vec4f result;
	_mm_store_ps(&result, mresult);
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


float inline v4_dot(Vec4f v1, Vec4f v2) {

#ifdef _INCLUDED_SMM 
	__m128 m1 = _mm_load_ps(&v1);
	__m128 m2 = _mm_load_ps(&v2);
	// 0xF1 == 0000 0000 1111 0001, meaning do the dot product on all 1111, and place the result at the bottom 0001
	__m128 mresult = _mm_dp_ps(m1, m2, 0xF1);
	float dot = _mm_cvtss_f32(mresult);
	return dot;
#else
	float dot = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	return dot;
#endif

}


float inline v4_length(Vec4f vec) {
	

#ifdef _INCLUDED_SMM 
	// This saves an extra load from v4_dot since, we are dotting m1 with itself
	__m128 m1 = _mm_load_ps(&vec);
	__m128 mresult = _mm_dp_ps(m1, m1, 0xF1);
	float length = _mm_cvtss_f32(_mm_sqrt_ss(mresult));
#elif _INCLUDED_MM2
	float length = v4_dot(vec, vec);
	__m128 mresult = _mm_sqrt_ss(_mm_set1_ps(length));
	length = _mm_cvtss_f32(mresult);
	return length;
#else
	float length = v4_dot(vec, vec);
	return sqrtf(length);
#endif
}

// NOTE: this is not as precise as it can be
// Since rsqrt is an approximate it is faster, but less precise. in the future if u need a more precise normalize
// create a funcition with sqrt
Vec4f inline v4_normalize(Vec4f vec) {

#ifdef _INCLUDED_SMM 
	__m128 m1 = _mm_load_ps(&vec);
	__m128 inverse_norm = _mm_rsqrt_ps(_mm_dp_ps(m1, m1, 0xFF));
	__m128 mresult = _mm_mul_ps(m1, inverse_norm);

	Vec4f result;
	_mm_store_ps(&result, mresult);
	return result;
#else

	float len = v4_length(vec);

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



Vec3f inline v3_add(Vec3f v1, Vec3f v2) {
	Vec3f result = v4_add(vec34pack(v1, v2)).xyz;
	return result;
}

Vec3f inline v3_negate(Vec3f vec) {
	Vec3f result = v4_negate(vec3f_to_vec4f(vec, 0)).xyz;
	return result;
}

Vec3f inline v3_sub(Vec3f v1, Vec3f v2) {
	Vec3f result = v4_sub(vec34pack(v1, v2)).xyz;
	return result;
}

Vec3f inline v3_multiply(float scalar, Vec3f v) {
	Vec3f result = v4_multiply(scalar, vec3f_to_vec4f(v, 0)).xyz;
	return result;
}


float inline v3_length(Vec3f vec) {
	float length = v4_length(vec3f_to_vec4f(vec, 0));
	return length;
}

Vec3f inline vec_normalize(Vec3f vec) {
	Vec3f result = v4_normalize(vec3f_to_vec4f(vec, 0)).xyz;
	return result;
}

float inline v3_dot(Vec3f v1, Vec3f v2) {
	float dot = v4_dot(vec34pack(v1, v2));
	return dot;
}

Vec3f inline v3_cross(Vec3f v1, Vec3f v2) {

#ifdef _INCLUDED_MM2
	Vec4f v41 = vec3f_to_vec4f(v1, 0);
	Vec4f v42 = vec3f_to_vec4f(v2, 0);

	__m128 m1 = _mm_load_ps(&v41);
	__m128 m2 = _mm_load_ps(&v42);
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
	_mm_store_ps(&result, mresult);
	return result.xyz;
#else
	Vec3f cross;
	cross.data[0] = v1.data[1] * v2.data[2] - v1.data[2] * v2.data[1];
	cross.data[1] = v1.data[2] * v2.data[0] - v1.data[0] * v2.data[2];
	cross.data[2] = v1.data[0] * v2.data[1] - v1.data[1] * v2.data[0];
	return cross; 
#endif
	
}
