#pragma once




#include <stdbool.h>
#include <SDL.h>

#include "../Math/Vec.h"
#include "../Math/Mat.h"
#include "../ObjFile.h"

#include "../Core/StaticMesh.h"
#include "../Core/Camera.h"
#include "../TextureData.h"

#define GLEW_STATIC
#include "glew.h"
#include  "SDL_opengl.h"



typedef struct OpenGLRenderer {
	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	Vec2i window_size;
	
	// TODO: move this to it's own place struture. use some sort of allocator
	StaticMesh mesh;
	SurfaceData texture;
					 
	GLuint VAO;
	GLuint VBO; // id to the Buffer that stores our vertices
	GLuint EBO; // id to the Buffer that stores our indices


	GLuint shader_program;
	GLuint vs;
	GLuint fs;

	GLuint textureID;

	Camera mainCamera;



} OpenGLRenderer;

static void load_shaders(OpenGLRenderer* opengl);

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);

void opengl_render(OpenGLRenderer* opengl);