#pragma once

#include "Core/Renderer/gl/GLShader.h"
#include <stdio.h>
#include  "SDL_opengl.h"
#include "debug_macros.h"
#include "Logger.h"

void load_gl_shader(GLShader* shader, const char** vertex_code, const char** fragment_code, const char** geometry_code) {



	GLint success;
	GLchar infoLog[512];

	shader->program = glCreateProgram();

	shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader->vertex_shader, 1, vertex_code, NULL);
	glCompileShader(shader->vertex_shader);
	// Print compile errors if any
	glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader->vertex_shader, 512, NULL, infoLog);
		LOG_FATAL("GLSHADER", "ERROR::SHADER::VERTEX::COMPILATION_FAILED, %s\n", infoLog);
	};
	glAttachShader(shader->program, shader->vertex_shader);
	glDeleteShader(shader->vertex_shader);


	shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader->fragment_shader, 1, fragment_code, NULL);
	glCompileShader(shader->fragment_shader);
	// Print compile errors if any
	glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader->fragment_shader, 512, NULL, infoLog);
		LOG_FATAL("GLSHADER", "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED, %s\n", infoLog);
	};
	glAttachShader(shader->program, shader->fragment_shader);
	glDeleteShader(shader->fragment_shader);

	// Test for the optional geometry shader
	if (geometry_code != NULL) {
		shader->fragment_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shader->geometry_shader, 1, geometry_code, NULL);
		glCompileShader(shader->geometry_shader);
		// Print compile errors if any
		glGetShaderiv(shader->geometry_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader->geometry_shader, 512, NULL, infoLog);
			LOG_FATAL("GLSHADER", "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED, %s\n", infoLog);
		};
		glAttachShader(shader->program, shader->geometry_shader);
		glDeleteShader(shader->geometry_shader);
	}

	glLinkProgram(shader->program);
	// Print linking errors if any

	glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader->program, 512, NULL, infoLog);
		LOG_FATAL("GLSHADER", "ERROR::SHADER::PROGRAM::LINKING_FAILED, %s\n", infoLog);
	}

	
}



void delete_gl_program(GLShader* shader) {
	glUseProgram(0);
	glDeleteProgram(shader->program);
}