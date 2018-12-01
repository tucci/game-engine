#pragma once

//#include "Core/ECS/Component/Camera.h"
//#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Lights.h"

#include "Core/ECS/Component/Skybox.h"
#include "Core/TextureData.h"

#include "Core/ECS/EntityManager.h"





typedef struct Scene {
	int scene_id;
	

	Entity entity_main_camera;

	int entity_mesh_list_count = 0;
	Entity* entity_mesh_list;

	Entity sink;
	//Entity entity_mesh_test;
	//Entity entity_mesh_test2;
	//Entity entity_mesh_test3;
	Entity entity_test_light;

	Camera* main_camera;
	
	HDR_SkyMap hdr_skymap;

	Texture2D albedo_map;
	Texture2D normal_map;
	Texture2D metallic_map;
	Texture2D roughness_map;
	Texture2D ao_map;
	
	
} Scene;


