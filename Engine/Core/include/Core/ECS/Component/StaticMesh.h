#pragma once

#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Component/Transform.h"


typedef struct StaticMesh {
	// TODO: move this to the entity stuff. transform shoudldnt be in the static mesh
	//Transform transform;

	int vertex_count;
	int index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec3f* color;
	Vec3i* indices;
	StaticMesh() {
		vertex_count = 0;
		index_count = 0;
		pos = NULL;
		normal = NULL;
		texcoords = NULL;
		color = NULL;
		indices = NULL;
	}
} StaticMesh;


void obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, StackAllocator* memory);

#include <unordered_map>

typedef struct StaticMeshManager {
	std::unordered_map<int, int> id_map;
	StaticMesh* meshes;
	int count;
} StaticMeshManager;
