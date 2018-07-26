#pragma once

#include <stdbool.h>


#include <SDL.h>
#include "Mat.h"
#include "Vec.h"
#include "ObjDef.h"


typedef struct Shader {
	ObjModel* model;// TODO: change this to vbo style
	Mat4x4f* transform;
	Vec4f pos[3];
	Vec2f uv[3];
	Vec3f normals[3];
} Shader;


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
	Shader shader;

	ObjModel model;

} Renderer;




Vec4f vertex_shader(Shader* shader, int face_id, int vertex_id);

bool fragment_shader(Shader* shader, Vec2f frag_coord, Vec4f* output_color);





bool init_renderer(SDL_Window* window, Renderer* renderer, Vec2i size);
bool init_z_buffer(Renderer* renderer);
void clear_z_buffer(Renderer* renderer);

void init_camera(Renderer* renderer);

void init_shader(Renderer* renderer);

bool destroy_renderer(Renderer* renderer);
void render(Renderer* r);
void debug_render(Renderer* r);


