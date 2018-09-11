#pragma once

#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/LinearAllocator.h"

#include "Core/Transform.h"


typedef struct StaticMesh {
	// TODO: move this to the entity stuff. transform shoudldnt be in the static mesh
	Transform transform;

	int vertex_count;
	int index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec3f* color;
	Vec3i* indices;
} StaticMesh;


void obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, LinearAllocator* memory);