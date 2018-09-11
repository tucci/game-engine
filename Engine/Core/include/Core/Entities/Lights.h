#pragma once

#include "Core/Transform.h"

typedef struct DirectionalLight {
	Vec3f direction;
} DirectionalLight;

typedef struct PointLight {
	Transform transform;
} PointLight;