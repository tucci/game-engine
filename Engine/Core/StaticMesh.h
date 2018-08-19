#pragma once

#include "../Math/Vec.h"
#include "../ObjFile.h"

#include "../Common/LinearAllocator.h"


typedef struct StaticMesh {
	int vertex_count;
	int index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec3f* color;
	Vec3i* indices;
} StaticMesh;


void obj_to_static_mesh(ObjModel* obj, StaticMesh* static_mesh, LinearAllocator* memory);