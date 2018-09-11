#pragma once


#include "Core/TextureData.h"
#include "Common/LinearAllocator.h"
#include "Core/StaticMesh.h"

typedef struct Skybox {
	SimpleTexture front;
	SimpleTexture back;
	SimpleTexture left;
	SimpleTexture right;
	SimpleTexture top;
	SimpleTexture bottom;
	StaticMesh cube;
} Skybox;



typedef struct HDR_SkyMap {

	HDRTexture map;
	StaticMesh cube;
} HDR_SkyMap;


void load_skybox(Skybox* skybox, LinearAllocator* mem,
							const char* front, const char* back,
							const char* left, const char* right,
							const char* top, const char* bottom);

void load_hdr_skymap(HDR_SkyMap* map, LinearAllocator* mem, const char* filename);