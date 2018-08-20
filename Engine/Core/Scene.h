#pragma once

#include "Entities/Camera.h"
#include "StaticMesh.h"
#include "TextureData.h"



typedef struct Scene {
	int scene_id;
	Camera main_camera;
	StaticMesh mesh_test;
	SimpleTexture texture_test;
	StaticMesh primative_test;
} Scene;


