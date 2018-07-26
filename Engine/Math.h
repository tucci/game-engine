#pragma once

#include "Vec.h"
// TODO: deprecate need for math lib, go full simd
#include <math.h>

#define PI 3.141592653589f
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(x) (((x)<0) ? -(x) : (x))

#define deg_to_rad(deg) ((deg) * PI / 180.0f)

// Converts radians to degrees.
#define rad_to_deg(rad) ((rad) * 180.0f / PI)






typedef struct BoundingBox2i {
	Vec2i min;
	Vec2i max;
} BoundingBox2i;





// Taken from https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
// Paper from https://www.cs.drexel.edu/~david/Classes/Papers/comp175-06-pineda.pdf
float inline edge_function(Vec3f a, Vec3f b, Vec3f c) {
	return -(b.y - a.y)*(c.x - a.x) + (b.x - a.x)*(c.y - a.y);	
}

/// TODO: look into https://fgiesen.wordpress.com/2013/01/14/min-max-under-negation-and-an-aabb-trick/
BoundingBox2i inline get_bounding_box_from_tri(Vec2f v0, Vec2f v1, Vec2f v2, float width, float height) {

	BoundingBox2i box;

	box.min.x = (int)MAX(MIN(v0.x, MIN(v1.x, v2.x)), 0);
	box.min.y = (int)MAX(MIN(v0.y, MIN(v1.y, v2.y)), 0);

	box.max.x = (int)MIN(MAX(v0.x, MAX(v1.x, v2.x)), width);
	box.max.y = (int)MIN(MAX(v0.y, MAX(v1.y, v2.y)), height);
	
	return box;
}

float inline clamp(float x, float min, float max) {
	if (x < min) {
		x = min;
	}
	else if (x > max) {
		x = max;
	}
	return x;
}


// Taken from real time collision detection
Vec3f inline barycentric(Vec3f a, Vec3f b, Vec3f c, Vec3f p) {
	
	Vec3f v0 = vec_sub(b, a);
	Vec3f v1 = vec_sub(c, a);
	Vec3f v2 = vec_sub(p, a);
	
	float d00 = vec_dot(v0, v0);
	float d01 = vec_dot(v0, v1);
	float d11 = vec_dot(v1, v1);
	float d20 = vec_dot(v2, v0);
	float d21 = vec_dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	Vec3f bc;
	
	bc.v = (d11 * d20 - d01 * d21) / denom;
	bc.w = (d00 * d21 - d01 * d20) / denom;
	bc.u = 1.0f - bc.v - bc.w;
	return bc;
}



float inline remap(float value, float from1, float to1, float from2, float to2) {
	float result = (value - from1) / (to1 - from1) * (to2 - from2) + from2;
	return result;
}