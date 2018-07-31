#pragma once

#include  "SDL_opengl.h"
#include "OpenGLRenderer.h"



bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size) {
	opengl->gl_context = SDL_GL_CreateContext(window);
	opengl->sdl_window = window;
	opengl->window_size = window_size;

	return true;
}

bool destroy_opengl_renderer(OpenGLRenderer* opengl) {
	SDL_GL_DeleteContext(opengl->gl_context);
	return true;
}

void opengl_render(OpenGLRenderer* opengl) {
	
	// now you can make GL calls.
	/* rotate a triangle around */
	glViewport(0, 0, opengl->window_size.x, opengl->window_size.y);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2i(0, 1);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2i(-1, -1);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2i(1, -1);
	glEnd();
	glFlush();


	
	SDL_GL_SwapWindow(opengl->sdl_window);
}