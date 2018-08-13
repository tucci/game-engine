#pragma once

#include <stdbool.h>


#include <SDL.h>
#include "../../Math/Mat.h" // TODO: figure out better way to do this. dont want ../.. stuff
#include "../../Math/Vec.h"
#include "../../ObjFile.h"
#include "../../TextureData.h"
#include "../../Core/Camera.h"
#include "../../Common/LinearAllocator.h"

#include "SoftwareRendererShader.h"



typedef struct SoftwareRenderer {
	Camera camera;

	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Window* sdl_window;
	Vec2i window_size;
	float* zbuffer;
	int zbuffer_size;

	void* renderer_memory;
	size_t renderer_memory_size;
	LinearAllocator renderer_allocator;

	
	
	SoftwareRendererShader shader;

	// TODO: find a better way to store these things
	ObjModel model;
	SimpleTexture texture;

} SoftwareRenderer;




static Vec4f vertex_shader(SoftwareRendererShader* shader, int face_id, int vertex_id);
static bool fragment_shader(SoftwareRendererShader* shader, Vec3f bary, Vec4f frag_coord, Vec4f* output_color);

bool init_software_renderer(SDL_Window* window, SoftwareRenderer* renderer, Vec2i size, void* parition_start, size_t partition_size);
static bool init_z_buffer(SoftwareRenderer* renderer);
static void clear_z_buffer(SoftwareRenderer* renderer);
static void init_shader(SoftwareRenderer* renderer);
bool destroy_software_renderer(SoftwareRenderer* renderer);
void software_render(SoftwareRenderer* r);
void software_debug_render(SoftwareRenderer* r);
void software_swap_buffer(SoftwareRenderer* r);


