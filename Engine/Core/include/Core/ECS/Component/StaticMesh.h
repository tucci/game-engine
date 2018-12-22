#pragma once


#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Entity.h"
#include "Common/Map.h"

typedef struct StaticMeshID {
	u64 id;
} StaticMeshID;


typedef struct StaticMesh {
	
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
} StaticMesh;



bool obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, StackAllocator* memory);

typedef struct StaticMeshManager {
	CompactMap<u64> id_map;
	u64* meshes;
	u64 count;
} StaticMeshManager;


void init_static_mesh_manager(StaticMeshManager* manager);
void destroy_static_mesh_manager(StaticMeshManager* manager);
void entity_add_mesh_component(StaticMeshManager* manager, Entity entity);
void entity_remove_mesh_component(StaticMeshManager* manager, Entity entity);
