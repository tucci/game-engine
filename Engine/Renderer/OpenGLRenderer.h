#pragma once

#include <stdbool.h>
#include <SDL.h>

#include "../Math/Vec.h"

typedef struct OpenGLRenderer {
	SDL_GLContext gl_context;
	SDL_Window* sdl_window;
	Vec2i window_size;
} OpenGLRenderer;

bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size);
bool destroy_opengl_renderer(OpenGLRenderer* opengl);

void opengl_render(OpenGLRenderer* opengl);