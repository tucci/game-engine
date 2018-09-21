#pragma once


#include "Math/Vec.h"

typedef struct DirectionalLight {
	Vec3f direction;
	Vec3f color;
} DirectionalLight;

typedef struct PointLight {
	Vec3f position;
	Vec3f color;
} PointLight;

