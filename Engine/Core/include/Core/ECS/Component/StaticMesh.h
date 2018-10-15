#pragma once

#include "Math/Vec.h"
#include "ObjFile.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Entity.h"
#include "Common/Map.h"


typedef struct StaticMesh {
	
	int vertex_count;
	int index_count;
	int normal_count;
	int uv_count;
	int uv_index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec2f* lightmap_texcoords;
	Vec3f* color;
	Vec3i* indices;
	StaticMesh() {
		vertex_count = 0;
		index_count = 0;
		normal_count = 0;
		uv_count = 0;
		uv_index_count = 0;
		pos = NULL;
		normal = NULL;
		texcoords = NULL;
		color = NULL;
		indices = NULL;
	}
} StaticMesh;


typedef struct FBX_Property {
	char type_code;
	union {
		struct {
			int32_t length;
			union {
				void* raw_binary_data;
				char* str_data;
			};
		} special;
		
		struct {
			union {
				int16_t Y_data;
				char C_data;
				float F_data;
				double D_data;
				int32_t I_data;
				int64_t L_data;
			};
		} primative;

		struct {
			uint32_t array_length;
			uint32_t encoding;
			uint32_t compressed_length;
			union {
				void* v_data;// used for generic allocation without knowing the type ahead of time
				char* b_data;
				float* f_data;
				double* d_data;
				int32_t* i_data;
				int64_t* l_data;
			};
		} array;
		
	};
	
} FBX_Property;




#define FBX_NULL_RECORD "\0\0\0\0\0\0\0\0\0\0\0\0\0"


typedef struct FBX_Node {
	uint32_t end_offset;
	uint32_t num_properties;
	uint32_t property_list_length;
	uint8_t name_length;
	char* name;
	FBX_Property* properties;

	

	uint32_t child_count;

	FBX_Node* first_child;
	FBX_Node* next_sibling;

} FBX_Node;

typedef struct FBX_Import_Data {
	bool successfully_imported;
	int static_mesh_count;
	StaticMesh** meshes;
} FBX_Import_Data;


bool obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, StackAllocator* memory);


FBX_Import_Data import_fbx(const char* filename, StackAllocator* memory, bool y_is_up);
void free_fbx_import(FBX_Import_Data* import);



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