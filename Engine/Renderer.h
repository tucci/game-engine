#pragma once

#include <stdbool.h>


#include <SDL.h>
#include "Mat.h"
#include "Vec.h"
#include "ObjDef.h"


typedef struct Camera {
	Vec4f pos;
	Vec3f dir;
	Vec3f rotation;

	float near;
	float far;
	float fov;
	float aspect_ratio;
} Camera;



typedef struct Renderer {
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Window* sdl_window;
	Vec2i window_size;
	float* zbuffer;
	int zbuffer_size;
	Camera camera;

	ObjModel model;
	int face_count;

} Renderer;


bool init_renderer(SDL_Window* window, Renderer* renderer, Vec2i size);
bool init_z_buffer(Renderer* renderer);
void clear_z_buffer(Renderer* renderer);

void init_camera(Renderer* renderer);

bool destroy_renderer(Renderer* renderer);
void render(Renderer* r);
void debug_render(Renderer* r);

