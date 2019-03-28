#pragma once


#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Entity.h"
#include "Common/Map.h"

struct StaticMeshID {
	u64 id;
};


struct StaticMesh {
	
	StaticMeshID id;
	int vertex_count;
	int index_count;
	
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec2f* lightmap_texcoords;
	Vec3f* color;

	Vec3i* indices;
	StaticMesh() {
		vertex_count = 0;
		index_count = 0;
		pos = NULL;
		normal = NULL;
		texcoords = NULL;
		indices = NULL;
		color = NULL;
		lightmap_texcoords = NULL;
	}
};



bool obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, StackAllocator* memory);

struct StaticMeshManager {
	CompactMap<u64> id_map;
	StaticMeshID* meshes;
	u64 count;
};


void init_static_mesh_manager(StaticMeshManager* manager);
void destroy_static_mesh_manager(StaticMeshManager* manager);
bool entity_add_mesh_component(StaticMeshManager* manager, Entity entity);
bool entity_remove_mesh_component(StaticMeshManager* manager, Entity entity);
