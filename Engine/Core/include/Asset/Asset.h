#pragma once

#include <stdint.h>
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Math/Mat.h"


typedef enum AssetType {
	AssetType_None,
	AssetType_Scene,
	AssetType_StaticMesh,
	AssetType_Texture,
	AssetType_Material
} AssetType;


typedef struct AssetImport_StaticMesh {
	Vec3f translation;
	Vec3f rotation;
	Vec3f scale;
	StaticMesh mesh;

} AssetImport_StaticMesh;

typedef struct AssetInfo {
	AssetType type;
	char* filename;
} AssetInfo;



typedef struct AssetSceneNode {
	char* name;
	uint32_t name_length;
	// NOTE: not sure if a node can have multiple parents
	// Looks like some nodes have multiple parents. for example a material node
	//struct AssetSceneNode* parent;

	uint32_t children_count;
	struct AssetSceneNode* first_child;

	struct AssetSceneNode* next_sibling;
	// Relative to the node's parent
	Mat4x4f transform;
	
	
	uint32_t mesh_count;
	uint32_t* meshes;

} AssetSceneNode;


typedef struct AssetScene {
	AssetSceneNode* root;

	uint32_t mesh_count;
	AssetImport_StaticMesh* meshes;

} AssetScene;


typedef struct Asset {
	uint64_t id;
	AssetType type;
	union {
		AssetScene scene;
		AssetImport_StaticMesh* import_mesh;
	};
} Asset;


void init_scene_node(AssetSceneNode* node, char* name, uint32_t name_length);
void set_scene_node_name(AssetSceneNode* node, char* name, uint32_t name_length);
void add_child_to_scene_node(AssetSceneNode* node, AssetSceneNode* child);
void set_scene_node_transform(AssetSceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot);
void set_scene_node_transform(AssetSceneNode* node, Mat4x4f transform);


