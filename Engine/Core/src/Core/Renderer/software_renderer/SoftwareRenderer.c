#pragma once


#include "Core/Renderer/software_renderer/SoftwareRenderer.h"
#include "Common/common_macros.h"


#define flip_y(height, y)  height - 1 - y


void set_scene_for_software_renderer(SoftwareRenderer* renderer, Scene* scene) {
	renderer->render_scene = scene;
}

static Vec4f vertex_shader(SoftwareRendererShader* shader, int face_id, int vertex_id) {

	
	Vec3i v_id = shader->model->v_id[face_id];
	shader->pos[vertex_id] = shader->model->verts[v_id.data[vertex_id]];

	Vec3i tex_id = shader->model->vt_id[face_id];
	shader->uv[vertex_id] = shader->model->texcoords[tex_id.data[vertex_id]];

	Vec3i normal_id = shader->model->vn_id[face_id];
	shader->normals[vertex_id] = shader->model->normals[normal_id.data[vertex_id]];
		
		
	
	return *shader->transform * shader->pos[vertex_id];
}

static bool fragment_shader(SoftwareRendererShader* shader, Vec3f bary, Vec4f frag_coord, Vec4f* output_color) {
	
	//SDL_Surface* surface = shader->texture->surface;
	SimpleTexture* texture = shader->texture;
	
	Vec2f uv = {
		bary.x * shader->uv[0].u + bary.y * shader->uv[1].u + bary.z * shader->uv[2].u,
		bary.x * shader->uv[0].v + bary.y * shader->uv[1].v + bary.z * shader->uv[2].v,
	
	};
	
	
	int x = (int)remap(uv.u, 0.0f, 1.0f, 0.0f, (float)texture->width);
	int y = (int)remap(uv.v, 0.0f, 1.0f, 0.0f, (float)texture->height);

	unsigned char* pixelOffset = texture->data + (x + y * texture->width) * texture->channels;
	unsigned char r = pixelOffset[0];
	unsigned char g = pixelOffset[1];
	unsigned char b = pixelOffset[2];
	//unsigned char a = texture->channels >= 4 ? pixelOffset[3] : 0xff;

	output_color->r = r;
	output_color->g = g;
	output_color->b = b;
	output_color->a = 1;

	return false; // discard
}

bool init_software_renderer(SDL_Window* window, SoftwareRenderer* renderer, Vec2i window_size, void* parition_start, size_t partition_size) {
	renderer->surface = SDL_GetWindowSurface(window);
	renderer->renderer = SDL_CreateSoftwareRenderer(renderer->surface);
	renderer->sdl_window = window;
	renderer->window_size = window_size;

	renderer->renderer_memory = parition_start;
	renderer->renderer_memory_size = partition_size;

	stack_alloc_init(&renderer->renderer_allocator, renderer->renderer_memory, renderer->renderer_memory_size);

	
	if (!init_z_buffer(renderer)) { return false; }

	/* Clear the rendering surface with the specified color */
	SDL_SetRenderDrawColor(renderer->renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer->renderer);
	//renderer->renderer = SDL_CreateRenderer(renderer->sdl_window, -1, SDL_RENDERER_ACCELERATED);

	//load_obj("Assets/obj/african_head.obj", &renderer->model);
	//load_texture("Assets/obj/african_head_diffuse.tga", &renderer->texture2);

	load_obj("Assets/obj/diablo3_pose.obj", &renderer->model);

	//const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";

	bool loaded_texture = load_texture(texture_file, &renderer->texture, &renderer->renderer_allocator, true);

	if (!loaded_texture) return false;

	//load_obj("teapot.obj", &renderer->model);
	//load_obj("cow.obj", &renderer->model);
	//load_obj("teddy.obj", &renderer->model);
	//load_obj("dodecahedron.obj", &renderer->model);
	
	init_shader(renderer);
	return true;
}


static bool init_z_buffer(SoftwareRenderer* renderer) {
	int window_size = renderer->window_size.x * renderer->window_size.y;
	renderer->zbuffer_size = window_size;
	size_t size = sizeof(float) * window_size;
	renderer->zbuffer = cast(float*)stack_alloc(&renderer->renderer_allocator, size, 1);
	if (renderer->zbuffer == NULL) {
		return false;
	}
	for (size_t i = 0; i < window_size; i++) {
		renderer->zbuffer[i] = INFINITY;
	}

	return true;
}

static void clear_z_buffer(SoftwareRenderer* renderer) {
	int window_size = renderer->window_size.x * renderer->window_size.y;
	for (size_t i = 0; i < window_size; i++) {
		renderer->zbuffer[i] = INFINITY;
	}
	
}



static void init_shader(SoftwareRenderer* renderer) {
	renderer->shader.model = &renderer->model;
	renderer->shader.texture = &renderer->texture;

}

bool destroy_software_renderer(SoftwareRenderer* renderer) {
	free_obj(&renderer->model);
	//linear_reset(&renderer->renderer_allocator);
	SDL_DestroyRenderer(renderer->renderer);
	return true;
}





void software_render(SoftwareRenderer* r) {
	
	
	

	SDL_Renderer* renderer = r->renderer;
	
	Camera camera = *r->render_scene->main_camera;


	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	
	int width = r->window_size.x;
	int height = r->window_size.y;

	


	
	Mat4x4f model_mat;
	
	

	Mat4x4f view_mat = camera.view_mat;
	view_mat = transpose(view_mat);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = model_mat * view_mat * projection_mat;



	Mat4x4f viewport_mat = viewport(0, 0, width, height, 0, 1);
	
	
	for (int i = 0; i < r->model.face_count; i++) {
		Vec4f pos[3];
		r->shader.transform = &mvp_mat;
		for (int j = 0; j < 3; j++) {
			pos[j] = vertex_shader(&r->shader, i, j);
			// Viewport
			pos[j] = viewport_mat * pos[j];
			
		}
		

	
	
		Vec3f v0Raster = pos[0].xyz;
		Vec3f v1Raster = pos[1].xyz;
		Vec3f v2Raster = pos[2].xyz;

		v0Raster.z = 1 / v0Raster.z;
		v1Raster.z = 1 / v1Raster.z;
		v2Raster.z = 1 / v2Raster.z;


		BoundingBox2i bounds = get_bounding_box_from_tri(
			pos[0].xyz.xy, 
			pos[1].xyz.xy,
			pos[2].xyz.xy, (float)width - 1.0f, (float)height - 1.0f);

		

		float area = edge_function(v0Raster, v1Raster, v2Raster);
		float one_over_area = 1.0f / area;


		for (int x = bounds.min.x; x <= bounds.max.x; x++) {
			for (int y = bounds.min.y; y <= bounds.max.y; y++) {
				Vec3f pt = 
				{ (float)x, (float)y, 0.0};
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
							SDL_SetRenderDrawColor(renderer, (Uint8) frag_color.r, (Uint8)frag_color.g, (Uint8)frag_color.b , SDL_ALPHA_OPAQUE);
							// Flip y coordiinate
							SDL_RenderDrawPoint(renderer, x, flip_y(height, y));
						}

					}
				}
			}

		}

	}


	
	
}


void software_debug_render(SoftwareRenderer* r) {
	SDL_Renderer* renderer = r->renderer;
	Camera camera = *r->render_scene->main_camera;

	
	/*SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);*/



	//SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);


	int width = r->window_size.x;
	int height = r->window_size.y;




	
	Mat4x4f model_mat;
	
	

	Mat4x4f view_mat = camera.view_mat;
	view_mat = transpose(view_mat);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	// TODO: this is in the wrong order
	Mat4x4f mvp_mat = model_mat * view_mat * projection_mat;



	Mat4x4f viewport_mat = viewport(0, 0, width, height, 0, 1);

	Mat4x4f mat = mvp_mat * viewport_mat;
	
	float axis_scale = 10.0f;
	Vec4f x_axis = { axis_scale, 0, 0, 1 };
	Vec4f y_axis = { 0, axis_scale, 0, 1 };
	Vec4f z_axis = { 0, 0, axis_scale, 1 };


	Vec4f originf = { 0, 0, 0, 1 };
	originf = mat * originf;
	Vec4i origin = Vec4i(originf);

	
	Vec4i axis_pt;

	// Draw x axis
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	axis_pt = Vec4i(mat * x_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y) , axis_pt.x, flip_y(height, axis_pt.y));


	// Draw y axis
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	axis_pt = Vec4i(mat * y_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y), axis_pt.x, flip_y(height, axis_pt.y));


	// Draw z axis
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
	axis_pt = Vec4i(mat * z_axis);
	SDL_RenderDrawLine(renderer, origin.x, flip_y(height, origin.y), axis_pt.x, flip_y(height, axis_pt.y));



	int size = 16;
	

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	for (int i = -size; i <= size; i++) {
		float size_f = 1.0f * size;
		float i_f = 1.0f * i;

		Vec4f ptf = { -size_f, 0, i_f, 1 };
		Vec4f pt2f = { size_f, 0, i_f, 1 };

		Vec4i pt;
		Vec4i pt2;

		pt = Vec4i(mat * ptf);
		pt2 = Vec4i(mat * pt2f);

		SDL_RenderDrawLine(renderer, pt.x, flip_y(height, pt.y), pt2.x, flip_y(height, pt2.y));


		ptf =  Vec4f( i_f, 0, -size_f, 1 );
		pt2f = Vec4f( i_f, 0, size_f, 1 );

		pt = Vec4i((mat * ptf));
		pt2 = Vec4i(mat * pt2f);
		SDL_RenderDrawLine(renderer, pt.x, flip_y(height, pt.y), pt2.x, flip_y(height, pt2.y));

	}

	
}

void software_swap_buffer(SoftwareRenderer* r) {
	SDL_UpdateWindowSurface(r->sdl_window);
	clear_z_buffer(r);
}

