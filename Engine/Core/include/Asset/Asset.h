#pragma once

#include "types.h"
#include "Common/Arena.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
//#include "Math/Mat.h"


typedef enum AssetType {
	AssetType_None,
	AssetType_Scene,
	AssetType_StaticMesh,
	AssetType_Material,
	AssetType_Light,
	AssetType_Camera,
	AssetType_Animation,
	AssetType_Texture,
} AssetType;



typedef struct SceneID {
	u64 id;
} SceneId;

typedef struct StaticMeshID {
	u64 id;
} StaticMeshID;

typedef struct MaterialID {
	u64 id;
} MaterialID;

typedef struct LightID {
	u64 id;
} LightID;

typedef struct CameraID {
	u64 id;
} CameraID;

typedef struct AnimationID {
	u64 id;
} AnimationID;

typedef struct TextureID {
	u64 id;
} TextureID;


typedef struct AssetID {
	AssetType type;
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
} AssetID;





typedef struct AssetImport_StaticMesh {
	Vec3f translation;
	Vec3f rotation;
	Vec3f scale;
	StaticMesh mesh;

} AssetImport_StaticMesh;

typedef struct AssetImport_SceneNode {
	// NOTE: this id is not the same as asset ids
	u64 id;
	char* name;
	u32 name_length;

	// Relative to the node's parent
	//Mat4x4f transform;
	Vec3f translation;
	Vec3f scale;
	Vec3f rotation;

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

} AssetSceneNode;



typedef struct AssetImport_Scene {
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

} AssetScene;


typedef enum AssetStatus {
	AssetStatus_Unloaded,
	AssetStatus_Queued,
	AssetStatus_Loaded
} AssetStatus;



typedef struct AssetTrackData {
	u32 filename_length;
	char* filename;
} AssetTrackData;


#define ASSET_TRACKER_FILE "Assets.trak"
#define ASSET_TRACKER_TMP_SWAP_FILE "Assets.trak.tmp"

typedef struct AssetTracker {
	Arena mem;
	u64 last_asset_id = 1;
	u64 assets_tracked;
	CompactMap<AssetTrackData> track_map;
	
} AssetTracker;




void init_asset_tracker(AssetTracker* tracker);
void destroy_asset_tracker(AssetTracker* tracker);

// trying to look for assets by name is very slow
// find_asset_by_name and is_asset_tracked both do the same thing
// is_asset_tracked is just a convience function over find_asset_by_name
bool is_asset_tracked(AssetTracker* tracker, char* filename);
AssetID find_asset_by_name(AssetTracker* tracker, const char* filename);
AssetID track_asset(AssetTracker* tracker, char* filename, u32 filename_length);
static u64 next_asset_id(AssetTracker* tracker);

void write_tracker_file(AssetTracker* tracker);


void init_scene_node(AssetImport_SceneNode* node, u64 id, char* name, u32 name_length);
void set_scene_node_name(AssetImport_SceneNode* node, char* name, u32 name_length);
void add_child_to_scene_node(AssetImport_SceneNode* node, AssetImport_SceneNode* child);
void set_scene_node_transform(AssetImport_SceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot);


