#pragma once

#include <stdint.h>
#include "Core/ECS/Component/StaticMesh.h"

typedef enum AssetType {
	AssetType_None,
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
	char* filename;
} AssetInfo;



typedef struct AssetSceneNode {
	char* name;
	uint32_t name_length;

	struct AssetSceneNode* parent;

	uint32_t children_count;
	struct AssetSceneNode** children;
	
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
		AssetImport_StaticMesh* import_mesh;
	};
} Asset;


