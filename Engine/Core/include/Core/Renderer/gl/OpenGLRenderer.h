#pragma once




#include <stdbool.h>
#include <SDL.h>

#include "Math/Vec.h"
#include "Math/Mat.h"


#include "Core/Renderer/RenderInterface.h"

#include "Common/Arena.h"
#include "Common/StackAllocator.h"
#include "Common/Map.h"

#include "Core/Scene.h"
#include "Core/ECS/Component/StaticMesh.h"

#include "GLShader.h"

#define GLEW_STATIC
#include "glew.h"
#include  "SDL_opengl.h"


#define DEBUG_GRID_SIZE 100

#define SHADOW_WIDTH_RES 1024
#define SHADOW_HEIGHT_RES 1024


#define RENDERER_MEMORY MEGABYTES(10)


typedef struct OpenGLRenderer {

	



	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	

	
	RenderWorld* render_world;

	Arena arena;
	StackAllocator stack_allocator;



	
	int obj_capacity;
	
	// TODO: we need to create our own stretchy buffs using our allocators
	int texture_count;
	GLuint* textures;

	int shader_count;
	GLShader* shaders;

	int vbo_count;
	GLuint* vbos;

	int ebo_count;
	GLuint* ebos;

	int vao_count;
	GLuint* vaos;

	int fbo_count;
	GLuint* fbos;

	int rbo_count;
	GLuint* rbos;
	

	// Debug
	StaticMesh grid_mesh;
	int axes_pos_offset;
	bool show_debug_grid;
	bool show_debug_axes;
	bool draw_lines;


	
} OpenGLRenderer;


void gl_init_hdr_map(OpenGLRenderer* opengl, HDR_SkyMap* skymap);
void gl_init_shadow_maps(OpenGLRenderer* opengl);

// TODO: shadowmaps, textures, hdr texture, cubemap should all be implemented into one function
// gl_create_texture, with a paramter that specifcies what type it is
RenderResource gl_create_texture(OpenGLRenderer* opengl, Texture2D* texture, bool mipmap);
RenderResource gl_create_shadow_map(OpenGLRenderer* opengl, unsigned int width, unsigned int height);
RenderResource gl_create_hdr_texture(OpenGLRenderer* opengl, HDRTexture* hdr_texture, GLenum format, GLint internalformat);
RenderResource gl_create_cubemap(OpenGLRenderer* opengl, unsigned int width, unsigned int height, bool mipmap);
RenderResource gl_create_shader(OpenGLRenderer* opengl, const char* vertex_file, const char* fragment_file);
RenderResource gl_create_vbo(OpenGLRenderer* opengl);
RenderResource gl_create_ebo(OpenGLRenderer* opengl);
RenderResource gl_create_vao(OpenGLRenderer* opengl);
RenderResource gl_create_fbo(OpenGLRenderer* opengl);
RenderResource gl_create_rbo(OpenGLRenderer* opengl, u32 width, u32 height);




void gizmo_render_axis(OpenGLRenderer* opengl, Vec3f pos, Vec3f forward, Vec3f up, Vec3f right);

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, RenderWorld* render_world);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);

void init_gl_resource_arrays(OpenGLRenderer* opengl);
void opengl_render(OpenGLRenderer* opengl, Vec2i viewport_size, bool render_debug);
void clear_render_world(OpenGLRenderer* opengl);
void opengl_debug_render(OpenGLRenderer* opengl, Vec2i viewport_size);
void opengl_swap_buffer(OpenGLRenderer* opengl);