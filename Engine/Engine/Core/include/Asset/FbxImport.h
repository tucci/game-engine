#pragma once



#include "types.h"

#include "zlib.h"

#include "Asset/Asset.h"
#include "Common/Arena.h"
#include "Common/StackAllocator.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/Material.h"











struct FBX_Geometry {
	

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
};

struct FBX_Model {
	Vec3f local_translation;
	Vec3f local_rotation;
	Vec3f local_scaling;

	s32 name_length;
	char* name;
	
};



struct FBX_Material {
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
	

};



struct FBX_Texture {
	s32 name_length;
	s32 filename_length;
	s32 relative_filename_length;
	TextureType texture_type;

	char* name;
	char* filename;
	char* relative_filename;


	
	Vec3f translation;

	// uv transforms
	Vec2f uv_translation;
	Vec2f uv_scaling;
	float uv_rotation;


};

struct FBX_LayeredTexture {
	char* name;
	s32 name_length;
	s32 layered_texture;
	s32 blend_modes;

	TextureType texture_type;
	double alphas;
};



enum class FBX_Object_Type {
	Geometry,
	Model,
	Material,
	Texture,
	LayeredTexture
};

struct FBX_Object {
	FBX_Object_Type type;
	union {
		FBX_Geometry* geometry;
		FBX_Model* model;
		FBX_Material* material;
		FBX_Texture* texture;
		FBX_LayeredTexture* layered_texture;
	};
	

};

struct FBX_GlobalSettings {
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

};


struct FBX_Property {
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

};





#define FBX_NULL_RECORD "\0\0\0\0\0\0\0\0\0\0\0\0\0"


struct FBX_Node {
	u32 end_offset;
	u32 num_properties;
	u32 property_list_length;
	u8 name_length;
	char* name;
	FBX_Property* properties;



	u32 child_count;

	FBX_Node* first_child;
	FBX_Node* next_sibling;

};



struct AssetImportGlobalSettings {
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
};

struct AssetImporter {
	Arena mem;
	StackAllocator stack;
	s32 stack_allocs_count;
	// The tracker is usually owned by the asset manager, and is passed to this
	AssetTracker* tracker;

	// TODO: remove the fbx global settings, and have our own custom settings data. for we are just using this to hold the axis data
	AssetImportGlobalSettings global_settings;
};



struct FBX_ImportData {
	AssetImport_Scene export_scene;
	CompactMap<FBX_Object> fbx_object_map;
	CompactMap<AssetImport_SceneNode*> scene_node_cache_map;
	AssetImport_SceneNode** material_node_cache;
	FBX_GlobalSettings global_settings;
	const char* import_path;
	s32 import_path_length;
};


void init_asset_importer(AssetImporter* importer, AssetTracker* tracker);
void destroy_asset_importer(AssetImporter* importer);




static AssetID fbx_convert_geo2static_mesh_and_export(AssetImporter* importer, FBX_Geometry* geo, Vec3f pos, Vec3f scale, Vec3f get_rotation, String path, String filename);
static AssetID fbx_convert_mat_and_export(AssetImporter* importer, FBX_ImportData* import_data,AssetImport_SceneNode* material, String path, String filename);
static AssetID fbx_convert_texture_and_export(AssetImporter* importer, FBX_Texture* texture, String path, String filename);
AssetID export_static_mesh(AssetImporter* importer, StaticMesh* mesh, Vec3f pos, Vec3f scale, Vec3f get_rotation, String path, String filename);
AssetID export_asset_scene(AssetImporter* importer, AssetImport_Scene* scene, String path, String filename);
AssetID import_fbx(AssetImporter* importer, char* filename, bool reimport);



