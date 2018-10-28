#pragma once

#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Entity.h"
#include "Common/Map.h"



typedef struct StaticMesh {
	
	int vertex_count;
	int index_count;
	//int normal_count;
	//int uv_count;
	//int uv_index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec2f* lightmap_texcoords;
	Vec3f* color;

	Vec3i* indices;
	StaticMesh() {
		vertex_count = 0;
		index_count = 0;
		//normal_count = 0;
		//uv_count = 0;
		//uv_index_count = 0;
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
	CompactMap<uint64_t> id_map;
	StaticMesh* meshes;
	uint64_t count;
} StaticMeshManager;


void init_static_mesh_manager(StaticMeshManager* manager);
void destroy_static_mesh_manager(StaticMeshManager* manager);
void entity_add_mesh_component(StaticMeshManager* manager, Entity entity);
void entity_remove_mesh_component(StaticMeshManager* manager, Entity entity);
void free_static_mesh(StaticMesh* mesh);