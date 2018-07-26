#pragma once

#include "Renderer.h"


#define flip_y(height, y)  height - 1 - y

Vec4f vertex_shader(Shader* shader, int face_id, int vertex_id) {

	

	
	Vec3i v_id = shader->model->v_id[face_id];
	shader->pos[vertex_id] = shader->model->verts[v_id.data[vertex_id] - 1];

	Vec3i tex_id = shader->model->vt_id[face_id];
	shader->uv[vertex_id] = shader->model->texcoords[tex_id.data[vertex_id] - 1];

	Vec3i normal_id = shader->model->vn_id[face_id];
	shader->normals[vertex_id] = shader->model->normals[normal_id.data[vertex_id] - 1];
		
		
	
	return mat4x4_vec_mul(shader->transform, shader->pos[vertex_id]);
}

bool fragment_shader(Shader* shader, Vec3f bary,  Vec4f frag_coord, Vec4f* output_color) {
	
	SDL_Surface* surface = shader->model->diffuse.surface;
	
	Vec2f uv = {
		bary.x * shader->uv[0].u + bary.y * shader->uv[1].u + bary.z * shader->uv[2].u,
		bary.x * shader->uv[0].v + bary.y * shader->uv[1].v + bary.z * shader->uv[2].v,
	
	};
	
	//shader->model->diffuse->pixels
	int x = (int)remap(uv.u, 0, 1, 0, surface->w);
	int y = (int)remap(uv.v, 0, 1, 0, surface->h);

	//Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	
	uint32_t pixel = *((uint32_t *)surface->pixels + y * surface->w + x);

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	SDL_GetRGB(pixel, surface->format, &r, &g, &b, &a);


	output_color->r = r;
	output_color->g = g;
	output_color->b = b;
	output_color->a = 1;

	return false; // discard
}

bool init_renderer(SDL_Window* window, Renderer* renderer, Vec2i window_size) {
	// NOTE:(steven) for now we are doing software rendering, eventually, we'll get back to hardware rendering
	renderer->surface = SDL_GetWindowSurface(window);
	renderer->renderer = SDL_CreateSoftwareRenderer(renderer->surface);
	renderer->sdl_window = window;
	renderer->window_size = window_size;
	
	init_z_buffer(renderer);
	init_camera(renderer);

	/* Clear the rendering surface with the specified color */
	SDL_SetRenderDrawColor(renderer->renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer->renderer);
	//renderer->renderer = SDL_CreateRenderer(renderer->sdl_window, -1, SDL_RENDERER_ACCELERATED);

	/*load_obj("african_head.obj", &renderer->model);
	load_image("african_head_diffuse.tga", &renderer->model.diffuse);*/

	load_obj("diablo3_pose.obj", &renderer->model);
	load_image("diablo3_pose_diffuse.tga", &renderer->model.diffuse);

	//load_obj("teapot.obj", &renderer->model);
	//load_obj("cow.obj", &renderer->model);
	//load_obj("teddy.obj", &renderer->model);
	//load_obj("dodecahedron.obj", &renderer->model);
	
	init_shader(renderer);
	return true;
}


bool init_z_buffer(Renderer* renderer) {
	// TODO: malloc check
	int window_size = renderer->window_size.x * renderer->window_size.y;
	renderer->zbuffer_size = window_size;
	size_t size = sizeof(float) * window_size;
	renderer->zbuffer = (float*) malloc(size);
	for (size_t i = 0; i < window_size; i++) {
		renderer->zbuffer[i] = INFINITY;
	}

	return true;
}

void clear_z_buffer(Renderer* renderer) {
	int window_size = renderer->window_size.x * renderer->window_size.y;
	for (size_t i = 0; i < window_size; i++) {
		renderer->zbuffer[i] = INFINITY;
	}
	
}

void init_camera(Renderer* renderer) {
	renderer->camera.pos = (Vec4f) { 0, 0, 10, 1 };
	renderer->camera.dir = (Vec3f) { 0, 0, 1 };
	renderer->camera.rotation = Vec3f_Zero;

	renderer->camera.near = 0.1f;
	renderer->camera.far = 100.0f;

	renderer->camera.fov = 120.0f;
	renderer->camera.aspect_ratio = (float)renderer->window_size.x / (float)renderer->window_size.y;

	
}



void init_shader(Renderer* renderer) {
	renderer->shader.model = &renderer->model;

}

bool destroy_renderer(Renderer* renderer) {
	free_obj(&renderer->model);
	free(renderer->zbuffer);
	SDL_DestroyRenderer(renderer->renderer);
	return true;
}





void render(Renderer* r) {
	
	
	

	SDL_Renderer* renderer = r->renderer;
	Camera camera = r->camera;


	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	
	int width = r->window_size.x;
	int height = r->window_size.y;

	Vec3f eye = camera.pos.xyz_;
	Vec3f dir = camera.dir;


	
	Mat4x4f model_mat = rotate(camera.rotation.y, Vec3f_Up);
	Mat4x4f rot2 = rotate(camera.rotation.z, Vec3f_Forward);
	model_mat = mat4x4_mul(&model_mat, &rot2);

	Mat4x4f view_mat = look_at(eye, vec_add(eye, dir), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = mat4x4_mul(&model_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &projection_mat);



	Mat4x4f viewport_mat = viewport(0, 0, width, height, 0, 1);
	
	Mat4x4f mat = mat4x4_mul(&mvp_mat, &viewport_mat);
	
	
	
	
	for (int i = 0; i < r->model.face_count; i++) {
		Vec4f pos[3];
		r->shader.transform = &mvp_mat;
		for (int j = 0; j < 3; j++) {
			pos[j] = vertex_shader(&r->shader, i, j);
			// Viewport
			pos[j] = mat4x4_vec_mul(&viewport_mat, pos[j]);
			
		}
		

	
	
		Vec3f v0Raster = pos[0].xyz_;
		Vec3f v1Raster = pos[1].xyz_;
		Vec3f v2Raster = pos[2].xyz_;

		v0Raster.z = 1 / v0Raster.z;
		v1Raster.z = 1 / v1Raster.z;
		v2Raster.z = 1 / v2Raster.z;


		BoundingBox2i bounds = get_bounding_box_from_tri(
			pos[0].xyz_.xy_, 
			pos[1].xyz_.xy_,
			pos[2].xyz_.xy_, width - 1, height - 1);

		

		float area = edge_function(v0Raster, v1Raster, v2Raster);
		float one_over_area = 1.0f / area;


		for (int x = bounds.min.x; x <= bounds.max.x; x++) {
			for (int y = bounds.min.y; y <= bounds.max.y; y++) {
				Vec3f pt = (Vec3f) { x, y, 0.0};
				Vec3f bc = {
					edge_function(v1Raster, v2Raster, pt),
					edge_function(v2Raster, v0Raster, pt),
					edge_function(v0Raster, v1Raster, pt)
				};

				if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
					
					
					bc.x *= one_over_area;
					bc.y *= one_over_area;
					bc.z *= one_over_area;

					float one_over_z = v0Raster.z * bc.x + v1Raster.z * bc.y + v2Raster.z * bc.z;
					float z = 1 / one_over_z;

					int index = width * y + x;

					// z buffer check
					if (z < r->zbuffer[index]) {

						Vec4f frag_coord = { pt.x, pt.y, z, 1};
						Vec4f frag_color;
						bool discard_fragment = fragment_shader(&r->shader, bc, frag_coord, &frag_color);
						if (!discard_fragment) {
							r->zbuffer[index] = z;

							
							SDL_SetRenderDrawColor(renderer, frag_color.r, frag_color.g, frag_color.b , SDL_ALPHA_OPAQUE);
							//float depth = remap(z, -1, 1, 0, 255);
							//printf("depth %f\n", depth);
							//SDL_SetRenderDrawColor(renderer, depth, depth, depth, SDL_ALPHA_OPAQUE);
							// Flip y coordiinate
							SDL_RenderDrawPoint(renderer, x, flip_y(height, y));
						}

					}
				}
			}

		}

	}


	SDL_UpdateWindowSurface(r->sdl_window);
	clear_z_buffer(r);
	
}


void debug_render(Renderer* r) {
	SDL_Renderer* renderer = r->renderer;
	Camera camera = r->camera;

	
	/*SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);*/



	//SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);


	int width = r->window_size.x;
	int height = r->window_size.y;

	Vec3f eye = camera.pos.xyz_;
	Vec3f dir = camera.dir;




	Mat4x4f model_mat = rotate(camera.rotation.y, Vec3f_Up);
	Mat4x4f rot2 = rotate(camera.rotation.z, Vec3f_Forward);
	model_mat = mat4x4_mul(&model_mat, &rot2);

	Mat4x4f view_mat = look_at(eye, vec_add(eye, dir), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = mat4x4_mul(&model_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &projection_mat);



	Mat4x4f viewport_mat = viewport(0, 0, width, height, 0, 1);

	Mat4x4f mat = mat4x4_mul(&mvp_mat, &viewport_mat);
	
	float axis_scale = 10.0f;
	Vec4f x_axis = { axis_scale, 0, 0, 1 };
	Vec4f y_axis = { 0, axis_scale, 0, 1 };
	Vec4f z_axis = { 0, 0, axis_scale, 1 };


	Vec4f origin = { 0, 0, 0, 1 };
	origin = mat4x4_vec_mul(&mat, origin);

	// Draw x axis
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	Vec4f axis_pt = mat4x4_vec_mul(&mat, x_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y) , axis_pt.x, flip_y(height, axis_pt.y));


	// Draw y axis
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	axis_pt = mat4x4_vec_mul(&mat, y_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y), axis_pt.x, flip_y(height, axis_pt.y));


	// Draw z axis
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
	axis_pt = mat4x4_vec_mul(&mat, z_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y), axis_pt.x, flip_y(height, axis_pt.y));



	int size = 16;
	

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	for (int i = -size; i <= size; i++) {
		Vec4f pt = { -size,  0,i,1 };
		Vec4f pt2 = { size,  0,i,1 };

		pt = mat4x4_vec_mul(&mat, pt);
		pt2 = mat4x4_vec_mul(&mat, pt2);
		SDL_RenderDrawLine(renderer, pt.x, flip_y(height, pt.y), pt2.x, flip_y(height, pt2.y));


		pt = (Vec4f) { i ,0, -size, 1 };
		pt2 = (Vec4f) { i,0, size , 1 };

		pt = mat4x4_vec_mul(&mat, pt);
		pt2 = mat4x4_vec_mul(&mat, pt2);
		SDL_RenderDrawLine(renderer, pt.x, flip_y(height, pt.y), pt2.x, flip_y(height, pt2.y));

	}




	SDL_UpdateWindowSurface(r->sdl_window);
	clear_z_buffer(r);
}

