#pragma once



#include <stdint.h>

#include "zlib.h"

#include "Asset/Asset.h"
#include "Common/Arena.h"
#include "Common/StackAllocator.h"
#include "Core/ECS/Component/StaticMesh.h"



#define ASSET_FILE_EXTENSION ".easset"
#define ASSET_FILE_EXTENSION_LENGTH 7







typedef struct FBX_Geometry_Object {
	

	int vertex_count;
	int index_count;
	int normal_count;
	int uv_count;
	int uv_index_count;

	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;

	Vec3i* indices;
	Vec3i* uv_indices;
}FBX_Geometry_Object;

typedef struct FBX_Model_Object {
	Vec3f local_translation;
	Vec3f local_rotation;
	Vec3f local_scaling;

	uint64_t name_length;
	char* name;
	
} FBX_Model_Object;

typedef enum FBX_Object_Type {
	FBX_Object_Type_Geometry,
	FBX_Object_Type_Model,
} FBX_Object_Type;

typedef struct FBX_Object {
	FBX_Object_Type type;
	union {
		FBX_Geometry_Object* geo;
		FBX_Model_Object* model;
	};
	

} FBX_Object;


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




typedef struct AssetImporter {
	Arena mem;
	StackAllocator stack;
	int32_t stack_allocs_count;
	

	
	//int32_t asset_info_import_count;
	//AssetInfo* assets_infos;
	

} AssetImporter;



typedef struct FBX_ImportData {
	AssetScene export_scene;
	CompactMap<FBX_Object> fbx_object_map;
	CompactMap<AssetSceneNode*> scene_node_cache_map;
	
	bool y_is_up;
} FBX_ImportData;


void init_asset_importer(AssetImporter* importer);
void destroy_asset_importer(AssetImporter* importer);

AssetInfo export_static_mesh(AssetImporter* importer,
	StaticMesh* mesh,
	Vec3f pos,
	Vec3f rotation,
	Vec3f scale,
	char* filename, int filename_str_len);




AssetInfo import_fbx(AssetImporter* importer, const char* filename, bool y_is_up);



