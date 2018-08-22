#pragma once


#include "TextureData.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include "../Common/stb_image.h"


void fill_texture_info(const char* filename, SimpleTexture* texture) {
	texture->channels = STBI_rgb_alpha;
	stbi_info(filename, &texture->width, &texture->height, &texture->channels);
	// NOTE: hack, for some reason stbi info gets 3 channels, but when loading gets 4 channels
	// since we pre allocate the image chunk before calling stbi_load, we need the # of channels
	// this forces the allocator to load 4 channels per pixel
	texture->channels = STBI_rgb_alpha;

	if (texture->channels == STBI_rgb) {
		texture->depth = 24;
	} else { // STBI_rgb_alpha (RGBA)
		texture->depth = 32;
	}
}

// Returns 1 if loaded successfully, 0 if failed
bool load_and_copyto_texture(const char* filename, SimpleTexture* texture, bool flip) {
	int req_format = texture->channels;
	int orig_format = 0;

	// NOTE: Flip textures verticalu on load, our renderer expects textures to be upside down
	stbi_set_flip_vertically_on_load(flip);

	// Load the image
	unsigned char* data = stbi_load(filename, &texture->width, &texture->height, &orig_format, req_format);

	if (data == NULL) {
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
	}

	// Move the image to our own memory space, we wont the data to live in our allocator
	memcpy(texture->data, data, texture->width * texture->height * texture->channels);
	// Free the original data, we have the image in our own memory
	stbi_image_free(data);	
	return 1;
}


