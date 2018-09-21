#pragma once

#include <stdbool.h>


#include <SDL.h>
#include "Math/Mat.h"
#include "Math/Vec.h"

#include "Core/Scene.h"

#include "ObjFile.h"
#include "Core/TextureData.h"
#include "Core/ECS/Component/Camera.h"
#include "Common/LinearAllocator.h"

#include "Core/Renderer/software_renderer/SoftwareRendererShader.h"

// TODO: move this to it's own place
typedef struct BoundingBox2i {
	Vec2i min;
	Vec2i max;
} BoundingBox2i;

/// TODO: look into https://fgiesen.wordpress.com/2013/01/14/min-max-under-negation-and-an-aabb-trick/
BoundingBox2i inline get_bounding_box_from_tri(Vec2f v0, Vec2f v1, Vec2f v2, float width, float height) {

	BoundingBox2i box;

	box.min.x = (int)MAX(MIN(v0.x, MIN(v1.x, v2.x)), 0);
	box.min.y = (int)MAX(MIN(v0.y, MIN(v1.y, v2.y)), 0);

	box.max.x = (int)MIN(MAX(v0.x, MAX(v1.x, v2.x)), width);
	box.max.y = (int)MIN(MAX(v0.y, MAX(v1.y, v2.y)), height);

	return box;
}

// Taken from https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
// Paper from https://www.cs.drexel.edu/~david/Classes/Papers/comp175-06-pineda.pdf
float inline edge_function(Vec3f a, Vec3f b, Vec3f c) {
	return -(b.y - a.y)*(c.x - a.x) + (b.x - a.x)*(c.y - a.y);
}



// NOTE: Software renderer is deprecated and not updated anymore
// this was built to learn a basic level of the graphics pipeline
typedef struct SoftwareRenderer {
	Scene* render_scene;

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
	ObjModel model;
	SimpleTexture texture;

} SoftwareRenderer;



void set_scene_for_software_renderer(SoftwareRenderer* renderer, Scene* scene);
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


