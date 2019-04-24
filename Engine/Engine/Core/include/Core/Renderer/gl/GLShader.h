#pragma once

#ifndef GLEW_STATIC
	#define GLEW_STATIC
#endif // !GLEW_STATIC
#include "glew.h"


struct GLShader {
	GLuint program;
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint geometry_shader;
};


void load_gl_shader(GLShader* shader, const char** vertex_code, const char** fragment_code, const char** geometry_code);
void delete_gl_program(GLShader* shader);