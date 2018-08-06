#pragma once

#include "Vec.h"

typedef struct Transform {
	Vec3f position;
	Vec3f scale;
	Vec3f rotation; // TODO: implement as quaterions
} Transform;