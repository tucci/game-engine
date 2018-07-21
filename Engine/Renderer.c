#pragma once

#include "Renderer.h"


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

	load_obj("african_head.obj", &renderer->model);
	//load_obj("teapot.obj", &renderer->model);
	//load_obj("dodecahedron.obj", &renderer->model);
	renderer->face_count = stb_sb_count(renderer->model.faces);

	return true;
}


bool init_z_buffer(Renderer* renderer) {
	// TODO: malloc check
	int window_size = renderer->window_size.x * renderer->window_size.y;
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
	renderer->camera.pos = (Vec4f) { 0, 0, 0, 1 };
	renderer->camera.dir = (Vec3f) { 0, 0, -1 };
	renderer->camera.rotation = Vec3f_Zero;

	renderer->camera.near = 0.1f;
	renderer->camera.far = 100.0f;

	renderer->camera.fov = 120.0f;
	renderer->camera.aspect_ratio = (float)renderer->window_size.x / (float)renderer->window_size.y;

	
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



	//SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	
	int width = r->window_size.x;
	int height = r->window_size.y;

	Vec3f eye = camera.pos.xyz_;
	Vec3f dir = camera.dir;





	Mat4x4f model_mat = rotate(camera.rotation.y, Vec3f_Up);
	//Mat4x4f model_mat = mat4x4f_identity();
	Mat4x4f rot2 = rotate(camera.rotation.z, Vec3f_Forward);
	model_mat = mat4x4_mul(&model_mat, &rot2);

	Mat4x4f view_mat = look_at(eye, vec_add(eye, dir), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = mat4x4_mul(&model_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &projection_mat);

	Mat4x4f viewport_mat = viewport(0, 0, width, height, camera.far - camera.near);
	Mat4x4f mat = mat4x4_mul(&mvp_mat, &viewport_mat);
	
	clear_z_buffer(r);

	for (int i = 0; i < r->face_count; i++) {
		


		Vec3i face = r->model.faces[i];
		Vec4f tri[3] = {
			r->model.verts[face.data[0]-1],
			r->model.verts[face.data[1]-1],
			r->model.verts[face.data[2]-1]
		};

		SDL_SetRenderDrawColor(renderer, face.data[0] % 255, face.data[1] % 255, face.data[2] % 255, SDL_ALPHA_OPAQUE);
		//SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, SDL_ALPHA_OPAQUE);

		
		
		for (int j = 0; j < 3; j++) {
			// to raster space
			tri[j] =  mat4x4_vec_mul(&mat, tri[j]);
			//SDL_RenderDrawPoint(renderer, tri[j].x, tri[j].y);
			
		}
		/*for (int j = 0; j < 3; j++) {
			SDL_RenderDrawLine(renderer, tri[j].x, tri[j].y, tri[(j + 1) % 3].x, tri[(j + 1) % 3].y);

		}*/
		

	
	
		Vec3f v0Raster = tri[0].xyz_;
		Vec3f v1Raster = tri[1].xyz_;
		Vec3f v2Raster = tri[2].xyz_;


		BoundingBox2i bounds = get_bounding_box_from_tri(tri[0].xyz_.xy_, tri[1].xyz_.xy_, tri[2].xyz_.xy_);


		
		float area = edge_function(v0Raster, v1Raster, v2Raster);

		for (int x = bounds.min.x; x < bounds.max.x; x++) {
			for (int y = bounds.min.y; y < bounds.max.y; y++) {
				Vec3f pt = (Vec3f) { x + 0.5f, y + 0.5f, 0.0f };
				float w0 = edge_function(v1Raster, v2Raster, pt);
				float w1 = edge_function(v2Raster, v0Raster, pt);
				float w2 = edge_function(v0Raster, v1Raster, pt);
				
				if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
					w0 /= area;
					w1 /= area;
					w2 /= area;
					float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2;
					float z = 1 / oneOverZ;

					// z buffer check
					//if (z < r->zbuffer[width * y + x]) {
						//r->zbuffer[width * y + x] = z;
						/*float c = clamp(z, 0, 255);
						SDL_SetRenderDrawColor(renderer, c, c, c, SDL_ALPHA_OPAQUE);*/
						SDL_RenderDrawPoint(renderer, x, y);
					//}
				}
			}

		}
		
	}


	SDL_UpdateWindowSurface(r->sdl_window);

	//debug_render(r);
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
	
	//Mat4x4f model_mat = mat4x4f_identity();
	Mat4x4f view_mat = look_at(eye, vec_add(eye, dir), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = mat4x4_mul(&model_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &projection_mat);

	Mat4x4f viewport_mat = viewport(0, 0, width, height, camera.far - camera.near);
	Mat4x4f mat = mat4x4_mul(&mvp_mat, &viewport_mat);
	

	Vec4f x_axis = { 10.0f, 0, 0, 1 };
	Vec4f y_axis = { 0, 10.0f, 0, 1 };
	Vec4f z_axis = { 0, 0, 10.0f, 1 };


	Vec4f origin = { 0, 0, 0, 1 };
	origin = mat4x4_vec_mul(&mat, origin);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	Vec4f axis_pt = mat4x4_vec_mul(&mat, x_axis);
	SDL_RenderDrawLine(renderer, origin.x, origin.y, axis_pt.x, axis_pt.y);

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	axis_pt = mat4x4_vec_mul(&mat, y_axis);
	SDL_RenderDrawLine(renderer, origin.x, origin.y, axis_pt.x, axis_pt.y);

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
	axis_pt = mat4x4_vec_mul(&mat, z_axis);
	SDL_RenderDrawLine(renderer, origin.x, origin.y, axis_pt.x, axis_pt.y);



	int size = 8;
	int half_size = size / 2;
	// TODO: figure out if size should be a power of 2 for faster generation
	int div_size = 1;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	for (int i = -size; i <= size; i++) {
		Vec4f pt = { -size,  0,i,1 };
		Vec4f pt2 = { size,  0,i,1 };

		pt = mat4x4_vec_mul(&mat, pt);
		pt2 = mat4x4_vec_mul(&mat, pt2);
		SDL_RenderDrawLine(renderer, pt.x, pt.y, pt2.x, pt2.y);


		pt = (Vec4f) { i ,0, -size, 1 };
		pt2 = (Vec4f) { i,0, size , 1 };

		pt = mat4x4_vec_mul(&mat, pt);
		pt2 = mat4x4_vec_mul(&mat, pt2);
		SDL_RenderDrawLine(renderer, pt.x, pt.y, pt2.x, pt2.y);

	}




	SDL_UpdateWindowSurface(r->sdl_window);
}
