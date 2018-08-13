#pragma once


#include "SDL.h"


typedef struct SimpleTexture {
	unsigned char* data;
	int width;
	int height;
	int channels;
	int depth;
} SimpleTexture;


int load_texture(char* filename, SimpleTexture* texture);
void free_texture(SimpleTexture* texture);