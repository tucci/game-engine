#pragma once




#include <stdbool.h>
#include <SDL.h>

#include "../../Math/Vec.h"
#include "../../Math/Mat.h"




#include "../../Common/LinearAllocator.h"

#include "../../Core/Scene.h"
#include "../../Core/StaticMesh.h"

#include "GLShader.h"

#define GLEW_STATIC
#include "glew.h"
#include  "SDL_opengl.h"


#define DEBUG_GRID_SIZE 100



typedef struct OpenGLRenderer {

	Scene* render_scene;

	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	

	void* renderer_memory;
	size_t renderer_memory_size;
	LinearAllocator renderer_allocator;


	GLuint textureID;
	
	GLShader main_shader;
	GLuint VAO;
	GLuint VBO; // id to the Buffer that stores our vertices
	GLuint EBO; // id to the Buffer that stores our indices

	

	// Debug
	GLShader debug_shader;
	StaticMesh grid_mesh;
	int axes_pos_offset;
	bool show_debug_grid;
	bool show_debug_axes;

	GLuint grid_debug_VAO;
	GLuint grid_debug_VBO;
} OpenGLRenderer;


void set_scene_for_opengl_renderer(OpenGLRenderer* opengl, Scene* scene);
static void load_shaders(OpenGLRenderer* opengl);

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, void* parition_start, size_t partition_size);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);





void init_gl_debug(OpenGLRenderer* opengl);
void destroy_gl_debug(OpenGLRenderer* opengl);

void opengl_render(OpenGLRenderer* opengl, Vec2i viewport_size);
void opengl_debug_render(OpenGLRenderer* opengl, Vec2i viewport_size);
void opengl_swap_buffer(OpenGLRenderer* opengl);