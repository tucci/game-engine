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
	return (b.y - a.y)*(c.x - a.x) - (b.x - a.x)*(c.y - a.y);	
}

/// TODO: look into https://fgiesen.wordpress.com/2013/01/14/min-max-under-negation-and-an-aabb-trick/
BoundingBox2i inline get_bounding_box_from_tri(Vec2f v0, Vec2f v1, Vec2f v2) {

	BoundingBox2i box;

	box.max.x = (int)MAX(v0.x, MAX(v1.x, v2.x));
	box.min.x = (int)MIN(v0.x, MIN(v1.x, v2.x));
	box.max.y = (int)MAX(v0.y, MAX(v1.y, v2.y));
	box.min.y = (int)MIN(v0.y, MIN(v1.y, v2.y));
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
