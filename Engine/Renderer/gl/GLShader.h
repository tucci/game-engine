#pragma once

#include  "SDL_opengl.h"

typedef struct GLShader {
	GLuint program;
	GLuint vertex_shader;
	GLuint fragment_shader;
} GLShader;

