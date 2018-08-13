#pragma once


#include "TextureData.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include "Common/stb_image.h"


// Returns 1 if loaded successfully, 0 if failed
int load_texture(char* filename, SimpleTexture* texture) {
	// NOTE: Flip textures verticall on load, our renderer expects textures to be upside down
	stbi_set_flip_vertically_on_load(1);
	int req_format = STBI_rgb_alpha;
	int orig_format;
	texture->data = stbi_load(filename, &texture->width, &texture->height, &orig_format, req_format);
	if (texture->data == NULL) {
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
		return 0;
	}
	if (req_format == STBI_rgb) {
		texture->depth = 24;
		texture->channels = 3;
	} else { // STBI_rgb_alpha (RGBA)
		texture->depth = 32;
		texture->channels = 4;
	}
	return 1;
}

void free_texture(SimpleTexture* texture) {
	stbi_image_free(texture->data);
}

