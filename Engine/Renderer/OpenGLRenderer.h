#pragma once

#include <stdbool.h>
#include <SDL.h>

#include "../Math/Vec.h"
#include "../ObjFile.h"

#define GLEW_STATIC
#include "glew.h"
#include  "SDL_opengl.h"

#include "../StaticMesh.h"

typedef struct OpenGLRenderer {
	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	Vec2i window_size;
	ObjModel model; // TODO: remove this
	StaticMesh mesh;

					 
	GLuint VAO;
	//// id to the Buffer that stores our vertices
	GLuint VBO;
	////// id to the Buffer that stores our indices
	GLuint EBO;


	GLuint shader_program;
	GLuint vs;
	GLuint fs;

	GLuint textureID;



} OpenGLRenderer;

static void load_shaders(OpenGLRenderer* opengl);

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);


void opengl_render(OpenGLRenderer* opengl);