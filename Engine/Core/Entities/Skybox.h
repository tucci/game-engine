#pragma once

#include "../TextureData.h"
#include "../../Common/LinearAllocator.h"
#include "../StaticMesh.h"

typedef struct Skybox {
	SimpleTexture front;
	SimpleTexture back;
	SimpleTexture left;
	SimpleTexture right;
	SimpleTexture top;
	SimpleTexture bottom;
	StaticMesh cube;
} Skybox;


void load_skybox(Skybox* skybox, LinearAllocator* mem,
							const char* front, const char* back,
							const char* left, const char* right,
							const char* top, const char* bottom);