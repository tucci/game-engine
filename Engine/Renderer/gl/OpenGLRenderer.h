#pragma once




#include <stdbool.h>
#include <SDL.h>

#include "../../Math/Vec.h"
#include "../../Math/Mat.h"
#include "../../ObjFile.h"

#include "../../Core/StaticMesh.h"
#include "../../Core/Camera.h"
#include "../../TextureData.h"

#include "GLShader.h"

#define GLEW_STATIC
#include "glew.h"
#include  "SDL_opengl.h"


#define DEBUG_GRID_SIZE 100

typedef struct OpenGLRenderer {
	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	Vec2i window_size;

	Camera main_camera;
	// TODO: move this to it's own place struture. use some sort of allocator
	StaticMesh mesh;
	SurfaceData texture;
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

static void load_shaders(OpenGLRenderer* opengl);

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);



void init_gl_debug(OpenGLRenderer* opengl);
void destroy_gl_debug(OpenGLRenderer* opengl);

void opengl_render(OpenGLRenderer* opengl);
void opengl_debug_render(OpenGLRenderer* opengl);
void opengl_swap_buffer(OpenGLRenderer* opengl);