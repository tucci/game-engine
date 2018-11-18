#pragma once



#include <intrin.h>
#include <xmmintrin.h>

#include "Math/Math.h"


typedef union Vec2i {
	struct {
		int x;
		int y;
	};
	float data[2];


	Vec2i() : x(0), y(0) {};
	Vec2i(int x, int y) : x(x), y(y) {};

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

	Vec2f() : x(0), y(0) {};
	Vec2f(float x, float y) : x(x), y(y) {};
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
		Vec2f xy;
		float z;
	};
	
	struct {
		Vec2f uv;
		float w;
	};
	


	float data[3];

	
	Vec3f() : x(0), y(0), z(0) {};
	Vec3f(float x, float y, float z) : x(x), y(y), z(z) {};

} Vec3f;


typedef union Vec3i {

	struct {
		int x;
		int y;
		int z;
	};

	struct {
		Vec2i xy;
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

	Vec3i() : x(0), y(0), z(0) {};
	Vec3i(int x, int y, int z) : x(x), y(y), z(z) {};

} Vec3i;

typedef union Vec4f {
	struct {
		float x;
		float y;
		float z;
		float w;
	};

	struct {
		Vec3f xyz;
		float w;
	};

	struct {
		float r;
		float g;
		float b;
		float a;
	};

	struct {
		Vec3f rgb;
		float a;
	};
	
	float data[4];

	Vec4f() : x(0), y(0), z(0), w(0) {};
	Vec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	Vec4f(Vec3f v, float w) : xyz(v), w(w) {};


} Vec4f;


typedef union Vec4i {

	struct {
		int x;
		int y;
		int z;
		int w;
	};
	struct {
		Vec3i xyz;
		int w;
	};

	struct {
		int r;
		int g;
		int b;
		int a;
	};
	struct {
		Vec3i rgb;
		int a;
	};




	int data[4];

	Vec4i() : x(0), y(0), z(0), w(0) {};
	Vec4i(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {};
	Vec4i(float x, float y, float z, float w) : x((int)x), y((int)y), z((int)z), w((int)w) {};
	Vec4i(Vec4f v) : x((int)v.x), y((int)v.y), z((int)v.z), w((int)v.w) {};
	Vec4i(Vec3i v, int w) : xyz(v), w(w) {};

} Vec4i;


#define Vec3f_Zero Vec3f(0,0,0)
#define Vec3f_One Vec3f(1,1,1)
#define Vec3f_Up Vec3f(0,1,0)
#define Vec3f_Down Vec3f(0,-1,0)
#define Vec3f_Forward Vec3f(0,0,1)
#define Vec3f_Backward Vec3f(0,0,-1)
#define Vec3f_Right Vec3f(1,0,0)
#define Vec3f_Left Vec3f(-1,0,0)

#define Vec4f_Zero Vec4f(0,0,0,0)
#define Vec4f_One Vec4f(1,1,1,1)




// Vec 4 operations
Vec4f operator+(const Vec4f& v1, const Vec4f& v2);// Addition
Vec4f& operator+=(Vec4f& v1, const Vec4f& v);// Addition
bool operator==(const Vec4f& v1, const Vec4f& v2);// Equality
Vec4f operator-(const Vec4f& v1, const Vec4f& v2);// Subtraction
Vec4f operator-(const Vec4f& vec); // Negation
Vec4f operator*(float scalar, const Vec4f& vec);// Scalar multiplication
Vec4f operator*(const Vec4f& vec, float scalar);// Scalar multiplication

float magnitude(const Vec4f& vec);
Vec4f normalize(const Vec4f& vec);
float dot(const Vec4f& v1, const Vec4f& v2);


// Vec3f operator overloads
Vec3f operator+(const Vec3f& v1, const Vec3f& v2);// Addition
Vec3f& operator+=(Vec3f& v1, const Vec3f& v);// Addition
bool operator==(const Vec3f& v1, const Vec3f& v2);// Equality
Vec3f operator-(const Vec3f& v1, const Vec3f& v2);// Subtraction
Vec3f operator-(const Vec3f& v);// Negation
Vec3f operator*(float scalar, const Vec3f& v); // Scalar multiplication
Vec3f operator*(const Vec3f& v, float scalar); // Scalar multiplication
float magnitude(const Vec3f& vec);
Vec3f normalize(const Vec3f& vec);
float dot(const Vec3f& v1, const Vec3f& v2);
Vec3f cross(const Vec3f& v1, const Vec3f& v2);


Vec3f angle_axis_rotate(const Vec3f& v, const Vec3f& n, float rads);
Vec3f euler_to_vector(const Vec3f& euler_angles);

Vec3f swap_yz(const Vec3f& v);
Vec3f swap_xy(const Vec3f& v);
Vec3f swap_xz(const Vec3f& v);

