#pragma once

#include "Entities/Camera.h"
#include "Entities/Skybox.h"

#include "StaticMesh.h"
#include "TextureData.h"





// TODO: implement proper tree with root and child transforms 
typedef struct Scene {
	int scene_id;
	Camera main_camera;
	Skybox skybox;
	StaticMesh mesh_test;
	SimpleTexture texture_test;
	StaticMesh primative_test;
} Scene;


