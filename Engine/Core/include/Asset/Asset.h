#pragma once

#include <stdint.h>
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
	uint64_t id;
} SceneId;

typedef struct StaticMeshID {
	uint64_t id;
} StaticMeshID;

typedef struct MaterialID {
	uint64_t id;
} MaterialID;

typedef struct LightID {
	uint64_t id;
} LightID;

typedef struct CameraID {
	uint64_t id;
} CameraID;

typedef struct AnimationID {
	uint64_t id;
} AnimationID;

typedef struct TextureID {
	uint64_t id;
} TextureID;


typedef struct AssetID {
	AssetType type;
	union {
		uint64_t id;
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
	uint64_t id;
	char* name;
	uint32_t name_length;

	// Relative to the node's parent
	//Mat4x4f transform;
	Vec3f translation;
	Vec3f scale;
	Vec3f rotation;

	uint32_t children_count;
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
	
	
	
	uint32_t mesh_count;
	uint32_t* meshes;

} AssetSceneNode;



typedef struct AssetImport_Scene {
	AssetImport_SceneNode* root;

	uint32_t node_count;

	uint32_t mesh_count;
	uint32_t material_count;
	uint32_t light_count;
	uint32_t camera_count;
	uint32_t anim_count;
	uint32_t texture_count;

	
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
	uint32_t filename_length;
	char* filename;
} AssetTrackData;


#define ASSET_TRACKER_FILE "Assets.trak"

typedef struct AssetTracker {
	Arena mem;
	uint64_t last_asset_id = 1;
	uint64_t assets_tracked;
	CompactMap<AssetTrackData> track_map;
} AssetTracker;




void init_asset_tracker(AssetTracker* tracker);
void destroy_asset_tracker(AssetTracker* tracker);
void track_asset(AssetTracker* tracker, AssetID id, char* filename, uint32_t filename_length);
uint64_t next_asset_id(AssetTracker* tracker);

void update_tracker_file(AssetTracker* tracker);


void init_scene_node(AssetImport_SceneNode* node, uint64_t id, char* name, uint32_t name_length);
void set_scene_node_name(AssetImport_SceneNode* node, char* name, uint32_t name_length);
void add_child_to_scene_node(AssetImport_SceneNode* node, AssetImport_SceneNode* child);
void set_scene_node_transform(AssetImport_SceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot);


