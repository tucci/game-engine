#pragma once


#include "SDL.h"

typedef struct SurfaceData {
	SDL_Surface* surface;
	unsigned char* data;
} SurfaceData;


void load_image(char* filename, SurfaceData* sd);
void free_texture(SurfaceData* surfaceData);