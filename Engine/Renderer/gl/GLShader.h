#pragma once

#ifndef GLEW_STATIC
	#define GLEW_STATIC
#endif // !GLEW_STATIC
#include "glew.h"


typedef struct GLShader {
	GLuint program;
	GLuint vertex_shader;
	GLuint fragment_shader;
} GLShader;


void load_gl_shader(GLShader* shader, const char** vertex_code, const char** fragment_code);
void delete_gl_program(GLShader* shader);