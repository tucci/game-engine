#pragma once


#include "SDL.h"
#include <stdbool.h>

#include "../Common/LinearAllocator.h"

typedef struct SimpleTexture {
	unsigned char* data;
	int width;
	int height;
	int channels;
	int depth;
} SimpleTexture;


typedef struct HDRTexture {
	float* data;
	int width;
	int height;
	int channels;
	int depth;
} HDRTexture;


bool load_texture(const char* filename, SimpleTexture* texture, LinearAllocator* mem, bool flip);
bool load_hdr_texture(const char* filename, HDRTexture* texture, LinearAllocator* mem, bool flip);