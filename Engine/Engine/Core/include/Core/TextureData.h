#pragma once


#include "SDL.h"
#include <stdbool.h>
#include "Math/Vec.h"
#include "types.h"

#include "Common/Arena.h"
#include "Common/StackAllocator.h"

struct TextureID {
	u64 id;
};




struct Texture2D {
	TextureID id;
	s32 width;
	s32 height;
	s32 channels;
	s32 depth;

	unsigned char* data;

	Vec2f uv_translation;
	Vec2f uv_scaling;
	float uv_rotation;

	
};


struct HDRTexture {
	s32 width;
	s32 height;
	s32 channels;
	s32 depth;

	float* data;

	Vec2f uv_translation;
	Vec2f uv_scaling;
	float uv_rotation;

};


void init_texture_default(Texture2D* texture);

bool load_texture(const char* filename, Texture2D* texture, Arena* mem, bool flip);
bool load_texture(const char* filename, Texture2D* texture, StackAllocator* mem, bool flip);
bool load_hdr_texture(const char* filename, HDRTexture* texture, StackAllocator* mem, bool flip);