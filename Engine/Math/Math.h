#pragma once



#define PI 3.141592653589f
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(x) (((x)<0) ? -(x) : (x))

#define deg_to_rad(deg) ((deg) * PI / 180.0f)

// Converts radians to degrees.
#define rad_to_deg(rad) ((rad) * 180.0f / PI)


// Good for small floating pts, bad for large pts
#define F_EQUAL_ABS(x, y, EPSILON) (int)(ABS(x - y) <= EPSILON)
// Bad for small floating pts, Good for large pts
#define F_EQUAL_REL(x, y, EPSILON) (int)(ABS(x - y) <= EPSILON * MAX(ABS(x), ABS(y)) 
// A basic balance between relative and absolute comparision
#define F_EQUAL_BALANCE(x, y, EPSILON) (int)(ABS(x - y) <= EPSILON * MAX(1.0f, ABS(x), ABS(y))
// A more controlled floating pt equal with control for relative and absolute
#define F_EQUAL_CONTROL(x, y, relTol, absTol) (int)(ABS(x - y) <= MAX(absTol, relTol * MAX(ABS(x), ABS(y))))




// TODO: implement our own sinf/cosf



float inline clamp(float x, float min, float max) {
	if (x < min) {
		x = min;
	}
	else if (x > max) {
		x = max;
	}
	return x;
}




float inline remap(float value, float from1, float to1, float from2, float to2) {
	float result = (value - from1) / (to1 - from1) * (to2 - from2) + from2;
	return result;
}


// TODO: implement lerp