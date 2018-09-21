#pragma once

//#include "Core/ECS/Component/Camera.h"
//#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Lights.h"

#include "Core/ECS/Component/Skybox.h"
#include "Core/TextureData.h"

#include "Core/ECS/EntityManager.h"





typedef enum SceneNodeType {
	SceneNodeType_Root,
	SceneNodeType_Camera,
	SceneNodeType_StaticMesh,
} SceneNodeType;


typedef struct SceneNodeArray {
	int size;
	int capacity;
	struct SceneNode* nodes;
} SceneNodeArray;


typedef struct SceneNode {
	SceneNodeType type;

	struct SceneNode* parent;
	SceneNodeArray children;

	union {
		Camera camera;
		StaticMesh mesh;
	};
	

} SceneNode;









typedef struct Scene {
	int scene_id;
	
	// TODO: implement proper tree with root and child transforms 
	//SceneNode root;
	// LinearAllocator* scene_mem;

	


	Entity* entity_main_camera;
	Entity* entity_mesh_test;
	Entity* entity_mesh_test2;
	Entity* entity_mesh_test3;

	Camera* main_camera;
	DirectionalLight test_light;
	HDR_SkyMap hdr_skymap;
	SimpleTexture albedo_map;
	SimpleTexture normal_map;
	SimpleTexture metallic_map;
	SimpleTexture roughness_map;
	SimpleTexture ao_map;
	
	


	
} Scene;


