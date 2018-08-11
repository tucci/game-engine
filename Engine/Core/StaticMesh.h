#pragma once

#include "../Math/Vec.h"
#include "../ObjFile.h"


typedef struct StaticMesh {
	int vertex_count;
	int index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec3f* color;
	Vec3i* indices;
} StaticMesh;


void convert_to_static_mesh(ObjModel* obj, StaticMesh* static_mesh);
void free_static_mesh(StaticMesh* static_mesh);