#pragma once

#include "Asset/Asset.h"

#include "Core/ECS/Component/StaticMesh.h"
#include "Core/Material.h"
#include "Core/TextureData.h"
#include "Common/Arena.h"




struct InternalAsset {
	AssetID id;
	union {
		AssetImport_Scene* scene;
		StaticMesh* mesh;
		Material* material;
		Texture2D* texture;
	};
};



struct AssetManager {
	Arena asset_mem;
	StackAllocator stack;
	AssetID* assets;
	AssetTracker asset_tracker;
	CompactMap<u64> asset_id_map;

	// TODO: lights/cameras arent really assets, and should be removed from the manager, we can probably move them into scene	

	

	u64 _scene_count;
	u64 _static_mesh_count;
	u64 _material_count;
	u64 _light_count;
	u64 _camera_count;
	u64 _anim_count;
	u64 _texture_count;

	AssetImport_Scene** _scenes;
	StaticMesh** _static_meshes;
	Material** _materials;
	//Light** _lights;
	//Camera** _cameras;
	//Animation** _anims;
	Texture2D** _textures;
	

	AssetID default_mat;
	AssetID plane_mesh;
	AssetID cube_mesh;
	AssetID sphere_mesh;


};



void init_asset_manager(AssetManager* manager);
void destroy_asset_manager(AssetManager* manager);

InternalAsset get_asset_by_id(AssetManager* manager, AssetID id);
StaticMesh* get_static_mesh_by_id(AssetManager* manager, StaticMeshID id);
Material* get_material_by_id(AssetManager* manager, MaterialID id);

void import_pak_file(AssetManager* manager, char* pak_file);


AssetID load_asset_by_name(AssetManager* manager, char* filename);
void load_asset_by_id(AssetManager* manager, AssetID id);




MaterialID create_material_asset(AssetManager* manager, IString path, IString name, Material* mat);
//TextureID create_texture(AssetManager* manager, IString path, IString name, Texture2D* texture);
AssetID import_texture(AssetManager* manager, IString file, bool reimport);

