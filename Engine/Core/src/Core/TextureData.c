#pragma once


#include "Core/TextureData.h"

#include "Common/common_macros.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include "Common/stb_image.h"



bool load_texture(const char* filename, Texture2D* texture, Arena* mem, bool flip) {
	texture->channels = STBI_rgb_alpha;
	// Precheck for width and height, so we can tell our allocator how much memory to reserve
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

	int image_size = texture->width * texture->height * texture->channels;

	if (image_size == 0) {
		return false;
	}

	texture->data = cast(unsigned char*) arena_alloc(mem, image_size);

	int req_format = texture->channels;
	int orig_format = 0;

	// NOTE: Flip textures verticalu on load, our renderer expects textures to be upside down
	stbi_set_flip_vertically_on_load(flip);

	// Load the image
	// TODO: figure out a way to load straight into our own allocated memory, instead of memcpy 
	unsigned char* data = stbi_load(filename, &texture->width, &texture->height, &orig_format, req_format);

	if (data == NULL) {
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
	}

	// Move the image to our own memory space, we want the data to live in our allocator
	memcpy(texture->data, data, texture->width * texture->height * texture->channels);
	// Free the original data, we have the image in our own memory
	stbi_image_free(data);
	return true;
}

// Returns 1 if loaded successfully, 0 if failed
bool load_texture(const char* filename, Texture2D* texture, StackAllocator* mem, bool flip) {

	texture->channels = STBI_rgb_alpha;
	// Precheck for width and height, so we can tell our allocator how much memory to reserve
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

	int image_size = texture->width * texture->height * texture->channels;

	if (image_size == 0) {
		return false;
	}

	texture->data = cast(unsigned char*) stack_alloc(mem, image_size, 4);

	int req_format = texture->channels;
	int orig_format = 0;

	// NOTE: Flip textures verticalu on load, our renderer expects textures to be upside down
	stbi_set_flip_vertically_on_load(flip);

	// Load the image
	// TODO: figure out a way to load straight into our own allocated memory, instead of memcpy 
	unsigned char* data = stbi_load(filename, &texture->width, &texture->height, &orig_format, req_format);

	if (data == NULL) {
		SDL_Log("Loading image failed: %s", stbi_failure_reason());
	}

	// Move the image to our own memory space, we want the data to live in our allocator
	memcpy(texture->data, data, texture->width * texture->height * texture->channels);
	// Free the original data, we have the image in our own memory
	stbi_image_free(data);	
	return true;
}

bool load_hdr_texture(const char* filename, HDRTexture* texture, StackAllocator* mem, bool flip) {
	

	texture->channels = STBI_rgb;
	stbi_info(filename, &texture->width, &texture->height, &texture->channels);

	size_t image_size = texture->width * texture->height * texture->channels * sizeof(float);

	if (image_size == 0) {
		return false;
	}

	texture->data = cast(float*) stack_alloc(mem, image_size, 4);

	
	int n_comp = STBI_rgb;

	stbi_set_flip_vertically_on_load(flip);

	float *data = stbi_loadf(filename, &texture->width, &texture->height, &n_comp, 0);
	
	if (data == NULL) {
		SDL_Log("Loading hdr image failed: %s", stbi_failure_reason());
		return false;
	}
	
	// Move the image to our own memory space, we want the data to live in our allocator
	memcpy(texture->data, data, image_size);
	// Free the original data, we have the image in our own memory
	stbi_image_free(data);
	return true;
}


