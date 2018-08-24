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



bool load_texture(const char* filename, SimpleTexture* texture, LinearAllocator* mem, bool flip);