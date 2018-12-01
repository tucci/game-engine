#pragma once



#include "types.h"

#include "zlib.h"

#include "Asset/Asset.h"
#include "Common/Arena.h"
#include "Common/StackAllocator.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/Material.h"



#define ASSET_FILE_EXTENSION ".easset"
#define ASSET_FILE_EXTENSION_LENGTH 7







typedef struct FBX_Geometry {
	

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
}FBX_Geometry;

typedef struct FBX_Model {
	Vec3f local_translation;
	Vec3f local_rotation;
	Vec3f local_scaling;

	s32 name_length;
	char* name;
	
} FBX_Model;



typedef struct FBX_Material {
	MaterialShadingModel shading_model;

	s32 name_length;
	char* name;

	double emissive_factor;
	double ambient_factor;
	double diffuse_factor;
	double transparency_factor;
	double reflection_factor;
	double specular_factor;
	double shininess_exponent;

	Vec3f emissive_color;
	Vec3f ambient_color;
	Vec3f diffuse_color;
	Vec3f transparent_color;
	Vec3f reflection_color;
	Vec3f specular_color;
	

} FBX_Material;



typedef struct FBX_Texture {
	s32 name_length;
	s32 filename_length;
	s32 relative_filename_length;

	char* name;
	char* filename;
	char* relative_filename;


	
	Vec3f translation;

	// uv transforms
	Vec2f uv_translation;
	Vec2f uv_scaling;
	float uv_rotation;


} FBX_Texture;

typedef struct FBX_LayeredTexture {
	char* name;
	s32 name_length;

	TextureType texture_type;

	s32 layered_texture;
	s32 blend_modes;
	double alphas;
} FBX_LayeredTexture;



typedef enum FBX_Object_Type {
	FBX_Object_Type_Geometry,
	FBX_Object_Type_Model,
	FBX_Object_Type_Material,
	FBX_Object_Type_Texture,
	FBX_Object_Type_LayeredTexture
} FBX_Object_Type;

typedef struct FBX_Object {
	FBX_Object_Type type;
	union {
		FBX_Geometry* geometry;
		FBX_Model* model;
		FBX_Material* material;
		FBX_Texture* texture;
		FBX_LayeredTexture* layered_texture;
	};
	

} FBX_Object;

typedef struct FBX_GlobalSettings {
	int up_axis;
	int up_axis_sign;
	int front_axis;
	int front_axis_sign;
	int coord_axis;
	int coord_axis_sign;
	int original_up_axis;
	int original_up_axis_sign;


	double unit_scale_factor;
	double original_unit_scale_factor;


}FBX_GlobalSettings;


typedef struct FBX_Property {
	char type_code;
	union {
		struct {
			s32 length;
			union {
				void* raw_binary_data;
				char* str_data;
			};
		} special;

		struct {
			union {
				s16 Y_data;
				char C_data;
				float F_data;
				double D_data;
				s32 I_data;
				s64 L_data;
			};
		} primative;

		struct {
			u32 array_length;
			u32 encoding;
			u32 compressed_length;
			union {
				void* v_data;// used for generic allocation without knowing the type ahead of time
				char* b_data;
				float* f_data;
				double* d_data;
				s32* i_data;
				s64* l_data;
			};
		} array;

	};

} FBX_Property;





#define FBX_NULL_RECORD "\0\0\0\0\0\0\0\0\0\0\0\0\0"


typedef struct FBX_Node {
	u32 end_offset;
	u32 num_properties;
	u32 property_list_length;
	u8 name_length;
	char* name;
	FBX_Property* properties;



	u32 child_count;

	FBX_Node* first_child;
	FBX_Node* next_sibling;

} FBX_Node;


typedef struct FBXGlobalSettings {
	int up_axis;
	int up_axis_sign;

	int front_axis;
	int front_axis_sign;

	int coord_axis;
	int coord_axis_sign;

	int original_up_axis;
	int original_up_axis_sign;


	double unit_scale_factor;
	double original_unit_scale_factor;
} AssetImportGlobalSettings;

typedef struct AssetImporter {
	Arena mem;
	StackAllocator stack;
	s32 stack_allocs_count;
	// The tracker is usually owned by the asset manager, and is passed to this
	AssetTracker* tracker;

	// TODO: remove the fbx global settings, and have our own custom settings data. for we are just using this to hold the axis data
	FBXGlobalSettings global_settings;
} AssetImporter;




typedef struct FBX_ImportData {
	AssetImport_Scene export_scene;
	CompactMap<FBX_Object> fbx_object_map;
	CompactMap<AssetImport_SceneNode*> scene_node_cache_map;
	AssetSceneNode** material_node_cache;
	FBX_GlobalSettings global_settings;
} FBX_ImportData;


void init_asset_importer(AssetImporter* importer, AssetTracker* tracker);
void destroy_asset_importer(AssetImporter* importer);




static AssetID fbx_convert_geo2static_mesh_and_export(AssetImporter* importer, FBX_Geometry* mesh, Vec3f pos, Vec3f scale, Vec3f rotation, char* filename, int filename_str_len);
static AssetID fbx_convert_mat_and_export(AssetImporter* importer, FBX_ImportData* import_data,AssetImport_SceneNode* material, char* filename, int filename_str_len);
static AssetID fbx_convert_texture_and_export(AssetImporter* importer, FBX_Texture* texture, char* filename, int filename_str_len);
AssetID export_static_mesh(AssetImporter* importer, StaticMesh* mesh, Vec3f pos, Vec3f scale, Vec3f rotation, char* filename, int filename_str_len);
AssetID export_asset_scene(AssetImporter* importer, AssetImport_Scene* scene, const char* filename, int filename_str_len);
AssetID import_fbx(AssetImporter* importer, char* filename, bool reimport);



