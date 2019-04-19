#pragma once

#include "engine_platform.h"
#include "types.h"

#include "Common/Arena.h"

#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/Material.h"
#include "Core/TextureData.h"
#include "Core/ECS/Component/Lights.h"
#include "Core/ECS/Component/Camera.h"

//#include "Math/Mat.h"


#define ASSET_FILE_EXTENSION ".easset"
#define ASSET_FILE_EXTENSION_LENGTH 7


enum class AssetType {
	None,
	Scene,
	StaticMesh,
	Material,
	Light,
	Camera,
	Animation,
	Texture,
};

enum class AssetStatus {
	Unloaded,
	Queued,
	Loaded
};



struct SceneID {
	u64 id;
	SceneID() {
		id = 0;
	}
};



struct AnimationID {
	u64 id;
	AnimationID() {
		id = 0;
	}
};



struct AssetID {
	AssetType type;
	AssetStatus status;
	union {
		u64 id;
		SceneID scene;
		StaticMeshID mesh;
		MaterialID material;
		LightID light;
		CameraID camera;
		AnimationID animation;
		TextureID texture;
	};

	AssetID() {
		type = AssetType::None;
		status = AssetStatus::Unloaded;
		id = 0;
	}
};





struct AssetImport_StaticMesh {
	Vec3f translation;
	Vec3f get_rotation;
	Vec3f scale;
	StaticMesh mesh;

};

struct AssetImport_SceneNode {
	// NOTE: this id is not the same as asset ids
	u64 id;
	char* name;
	u32 name_length;

	// Relative to the node's parent
	//Mat4x4f transform;
	Vec3f translation;
	Vec3f scale;
	Vec3f get_rotation;

	u32 children_count;
	union {
		// This idiom is used when the children count is not known ahead of time
		// this is usually while we are building the node tree
		struct {
			struct AssetImport_SceneNode* first_child;
			struct AssetImport_SceneNode* next_sibling;
		};
		// This is used when the children count is node ahead of time, and want to alloc one big array
		struct AssetImport_SceneNode* children;
	};
	
	
	
	u32 mesh_count;
	u32* meshes;

	u32 material_count;
	u32* materials;

	u32 texture_count;
	u32* textures;

	

};



struct AssetImport_Scene {
	SceneID id;
	AssetImport_SceneNode* root;

	u32 node_count;
	u32 mesh_count;
	u32 material_count;
	u32 light_count;
	u32 camera_count;
	u32 anim_count;
	u32 texture_count;

	
	AssetID* mesh_infos;
	AssetID* material_infos;
	AssetID* light_infos;
	AssetID* camera_infos;
	AssetID* animation_infos;
	AssetID* texture_infos;

	

};






struct AssetTrackData {
	u32 filename_length;
	char* filename;
};


#define ASSET_TRACKER_FILE "Assets.trak"
#define ASSET_TRACKER_TMP_SWAP_FILE "Assets.trak.tmp"

struct AssetTracker {
	Arena mem;
	u64 last_asset_id = 1;
	u64 assets_tracked;
	CompactMap<AssetTrackData> track_map;

	//todo asset tree/file path
	
};




void init_asset_tracker(AssetTracker* tracker);
void destroy_asset_tracker(AssetTracker* tracker);

// trying to look for assets by name is very slow
// find_asset_by_name and is_asset_tracked both do the same thing
// is_asset_tracked is just a convience function over find_asset_by_name
bool is_asset_tracked(AssetTracker* tracker, char* filename);
void remove_all_tracked_assets(AssetTracker* tracker);
AssetID find_asset_by_name(AssetTracker* tracker, const char* filename);
AssetID track_asset(AssetTracker* tracker, char* filename, u32 filename_length);
static u64 next_asset_id(AssetTracker* tracker);

void write_tracker_file(AssetTracker* tracker);


void init_scene_node(AssetImport_SceneNode* node, u64 id, char* name, u32 name_length);
void set_scene_node_name(AssetImport_SceneNode* node, char* name, u32 name_length);
void add_child_to_scene_node(AssetImport_SceneNode* node, AssetImport_SceneNode* child);
void set_scene_node_transform(AssetImport_SceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot);


