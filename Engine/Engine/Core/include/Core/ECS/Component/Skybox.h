#pragma once


#include "Core/TextureData.h"
#include "Common/LinearAllocator.h"
#include "Core/ECS/Component/StaticMesh.h"

struct Skybox {
	Texture2D front;
	Texture2D back;
	Texture2D left;
	Texture2D right;
	Texture2D top;
	Texture2D bottom;
	StaticMesh cube;
};



struct HDR_SkyMap {

	HDRTexture map;
	StaticMesh cube;
};


void load_skybox(Skybox* skybox, StackAllocator* mem,
							const char* front, const char* back,
							const char* left, const char* right,
							const char* top, const char* bottom);

void load_hdr_skymap(HDR_SkyMap* map, StackAllocator* mem, const char* filename);