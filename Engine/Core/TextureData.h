#pragma once


#include "SDL.h"
#include <stdbool.h>

typedef struct SimpleTexture {
	unsigned char* data;
	int width;
	int height;
	int channels;
	int depth;
} SimpleTexture;



void fill_texture_info(const char* filename, SimpleTexture* texture);
bool load_and_copyto_texture(const char* filename, SimpleTexture* texture, bool flip);