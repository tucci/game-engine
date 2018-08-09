#pragma once


#include "TextureData.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include "Common/stb_image.h"





void load_image(char* filename, SurfaceData* sd) {

	// NOTE: Flip textures verticall on load, our texture mengineer expects textures to be upside down
	stbi_set_flip_vertically_on_load(1);
	int req_format = STBI_rgb_alpha;
	int width, height, orig_format;
	sd->data = stbi_load(filename, &width, &height, &orig_format, req_format);
	if (sd->data == NULL) {
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
		exit(1);
	}

	int depth, pitch;
	Uint32 pixel_format;
	if (req_format == STBI_rgb) {
		depth = 24;
		pitch = 3 * width; // 3 bytes per pixel * pixels per row
		pixel_format = SDL_PIXELFORMAT_RGB24;
	} else { // STBI_rgb_alpha (RGBA)
		depth = 32;
		pitch = 4 * width;
		pixel_format = SDL_PIXELFORMAT_RGBA32;
	}

	sd->surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)sd->data, width, height,
		depth, pitch, pixel_format);

	if (sd->surface == NULL) {
		SDL_Log("Creating surface failed: %s", SDL_GetError());
		stbi_image_free(sd->data);
	}

}



void free_texture(SurfaceData* surfaceData) {
	if (surfaceData->surface) {
		SDL_FreeSurface(surfaceData->surface);
		stbi_image_free(surfaceData->data);
	}

}

