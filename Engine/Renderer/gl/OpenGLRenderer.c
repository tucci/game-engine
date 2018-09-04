#pragma once


#include "OpenGLRenderer.h"
#include "../../debug_macros.h"
#include "../../Common/common_macros.h"

#include "../../utils.c"



static void init_hdr_map(OpenGLRenderer* opengl, Scene* scene) {
	
	int skymap_buffer_width = 512;
	int skymap_buffer_height = 512;

	glGenFramebuffers(1, &opengl->capture_FBO);
	glGenRenderbuffers(1, &opengl->capture_RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, opengl->capture_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, opengl->capture_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, skymap_buffer_width, skymap_buffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, opengl->capture_RBO);


	glGenTextures(1, &opengl->hdr_skymap_id);
	glBindTexture(GL_TEXTURE_2D, opengl->hdr_skymap_id);

	int width = scene->hdr_skymap.map.width;
	int height = scene->hdr_skymap.map.height;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, scene->hdr_skymap.map.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// TODO: we can free the image from our texture on memory, since it is in the gpu mem now

	
	glGenTextures(1, &opengl->env_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->env_cubemap);
	for (unsigned int i = 0; i < 6; ++i) {
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, skymap_buffer_width, skymap_buffer_height, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);





	//// Load skymap to equirectangular shader 
	const char* vertex_shader = file_to_str("Assets/shaders/hdr_skymap.vs", &opengl->renderer_allocator);
	const char* fragment_shader = file_to_str("Assets/shaders/hdr_skymap.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->equi_rect_shader, &vertex_shader, &fragment_shader);
	//// TODO: pop off stack allocator

	// skybox already in the memory
	//vertex_shader = file_to_str("Assets/shaders/hdr_skymap.vs", &opengl->renderer_allocator);
	fragment_shader = file_to_str("Assets/shaders/irradiance_conv.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->irradiance_conv_shader, &vertex_shader, &fragment_shader);
	//// TODO: pop off stack allocator


	//// Load skybox shader
	vertex_shader = file_to_str("Assets/shaders/skybox.vs", &opengl->renderer_allocator);
	fragment_shader = file_to_str("Assets/shaders/skybox.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->skybox_shader, &vertex_shader, &fragment_shader);
	//// TODO: pop off stack allocator

	

	

	Mat4x4f captureProjection = perspective(0.1f, 10.0f, 90.0f, 1.0f);
	
	Vec3f center = make_vec3f(0, 0, 0);
	Mat4x4f captureViews[] =
	{
		look_at(center, make_vec3f(1.0f,  0.0f,  0.0f), make_vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, make_vec3f(-1.0f,  0.0f,  0.0f),make_vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, make_vec3f(0.0f,  1.0f,  0.0f), make_vec3f(0.0f,  0.0f,  1.0f)),
		look_at(center, make_vec3f(0.0f, -1.0f,  0.0f), make_vec3f(0.0f,  0.0f, -1.0f)),
		look_at(center, make_vec3f(0.0f,  0.0f,  1.0f), make_vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, make_vec3f(0.0f,  0.0f, -1.0f), make_vec3f(0.0f, -1.0f,  0.0f))
	};




	glGenVertexArrays(1, &opengl->skybox_VAO);
	glGenBuffers(1, &opengl->skybox_VBO);
	glGenBuffers(1, &opengl->skybox_EBO);
	
	


	glBindVertexArray(opengl->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, scene->hdr_skymap.cube.vertex_count * sizeof(Vec3f), scene->hdr_skymap.cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	

	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate env map from equirectangular map
	glUseProgram(opengl->equi_rect_shader.program);
	glUniformMatrix4fv(glGetUniformLocation(opengl->equi_rect_shader.program, "projection"), 1, GL_FALSE, captureProjection.mat1d);

	glUniform1i(glGetUniformLocation(opengl->equi_rect_shader.program, "equirectangular_map"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengl->hdr_skymap_id);


	glViewport(0, 0, skymap_buffer_width, skymap_buffer_height); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->capture_FBO);

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glFrontFace(GL_CW);

	GLint i_uni = glGetUniformLocation(opengl->equi_rect_shader.program, "view");
	for (unsigned int i = 0; i < 6; ++i) {
	
	
		glUniformMatrix4fv(i_uni, 1, GL_FALSE, captureViews[i].mat1d);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, opengl->env_cubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->skybox_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			opengl->render_scene->hdr_skymap.cube.index_count * sizeof(Vec3i),
			opengl->render_scene->hdr_skymap.cube.indices,
			GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 3 * opengl->render_scene->hdr_skymap.cube.index_count, GL_UNSIGNED_INT, 0);
	}
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS); // set depth function back to default


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate irridiance map

	int conv_width = 32;
	int conv_height = 32;
	
	

	

	

	glGenTextures(1, &opengl->irradiance_map_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->irradiance_map_id);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, conv_width, conv_height, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, opengl->capture_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, opengl->capture_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, conv_width, conv_height);

	
	
	glUseProgram(opengl->irradiance_conv_shader.program);
	





	glUniformMatrix4fv(glGetUniformLocation(opengl->irradiance_conv_shader.program, "projection"), 1, GL_FALSE, captureProjection.mat1d);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->env_cubemap);
	glUniform1i(glGetUniformLocation(opengl->irradiance_conv_shader.program, "environment_map"), 0);
	
	
	

	
	
	
	glBindVertexArray(opengl->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, scene->hdr_skymap.cube.vertex_count * sizeof(Vec3f), scene->hdr_skymap.cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glFrontFace(GL_CW);

	glViewport(0, 0, conv_width, conv_height);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->capture_FBO);
	i_uni = glGetUniformLocation(opengl->irradiance_conv_shader.program, "view");
	for (unsigned int i = 0; i < 6; ++i) {
		glUniformMatrix4fv(i_uni, 1, GL_FALSE, captureViews[i].mat1d);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, opengl->irradiance_map_id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->skybox_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			opengl->render_scene->hdr_skymap.cube.index_count * sizeof(Vec3i),
			opengl->render_scene->hdr_skymap.cube.indices,
			GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 3 * opengl->render_scene->hdr_skymap.cube.index_count, GL_UNSIGNED_INT, 0);
	}
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS); // set depth function back to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		debug_print(err);
	}

	
}





void set_scene_for_opengl_renderer(OpenGLRenderer* opengl, Scene* scene) {
	// TODO: instead of passing the entire scene, we should pass a culled scene for faster rendering
	opengl->render_scene = scene;

	init_hdr_map(opengl, scene);
	



	opengl->shadow_width_res = SHADOW_WIDTH_RES;
	opengl->shadow_height_res = SHADOW_HEIGHT_RES;
	
	
	glGenFramebuffers(1, &opengl->shadow_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->shadow_fbo);

	glGenTextures(1, &opengl->shadow_map);
	glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, opengl->shadow_width_res, opengl->shadow_height_res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, opengl->shadow_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		debug_print("Frame buffer not created");
	}
		

	


	// Bind Model textures
	glGenTextures(1, &opengl->albedo_map_id);
	glBindTexture(GL_TEXTURE_2D, opengl->albedo_map_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->albedo_map.width, scene->albedo_map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->albedo_map.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	glGenTextures(1, &opengl->normal_map_id);
	glBindTexture(GL_TEXTURE_2D, opengl->normal_map_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->normal_map.width, scene->normal_map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->normal_map.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glGenTextures(1, &opengl->metallic_map_id);
	glBindTexture(GL_TEXTURE_2D, opengl->metallic_map_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->metallic_map.width, scene->metallic_map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->metallic_map.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glGenTextures(1, &opengl->roughness_map_id);
	glBindTexture(GL_TEXTURE_2D, opengl->roughness_map_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->roughness_map.width, scene->roughness_map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->roughness_map.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glGenTextures(1, &opengl->ao_map_id);
	glBindTexture(GL_TEXTURE_2D, opengl->ao_map_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->ao_map.width, scene->ao_map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->ao_map.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	




	// set up vertex buffers
	// Generate the vertex array
	glGenVertexArrays(1, &opengl->VAO);
	// Bind vertex array to buffer
	glBindVertexArray(opengl->VAO);
	// Generate element buffer object
	glGenBuffers(1, &opengl->EBO);
	// Generate vertex buffer object
	glGenBuffers(1, &opengl->VBO);
	// bind vertex buffer object to buffer
	glBindBuffer(GL_ARRAY_BUFFER, opengl->VBO);
	// bind element buffer object to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	
}


static void init_gl_extensions(OpenGLRenderer* opengl) {
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		debug_print("Failed to init GLEW\n");
	} else {
		debug_print("Succeeded to init GLEW\n");
	}
}

static void load_shaders(OpenGLRenderer* opengl) {

	// TODO: move shader up to scene, also the scene shader has to be graphics backended independant, ex gl/ d3d
	// TODO: load shaders using stack allocator

	// Alloc on stack
	const char* vertex_shader   = file_to_str("Assets/shaders/textured.vs", &opengl->renderer_allocator);
	const char* fragment_shader = file_to_str("Assets/shaders/textured.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->main_shader, &vertex_shader, &fragment_shader);
	// Free stack


	// Alloc on stack
	vertex_shader = file_to_str("Assets/shaders/shadows.vs", &opengl->renderer_allocator);
	fragment_shader = file_to_str("Assets/shaders/shadows.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->shadow_shader, &vertex_shader, &fragment_shader);
	// Free stack

}


void init_gl_debug(OpenGLRenderer* opengl) {
	opengl->show_debug_grid = true;
	opengl->show_debug_axes = true;
	opengl->draw_lines = false;
	

	const char* debug_vertex_shader = file_to_str("Assets/shaders/debug.vs", &opengl->renderer_allocator);
	const char* debug_fragment_shader = file_to_str("Assets/shaders/debug.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->debug_shader, &debug_vertex_shader, &debug_fragment_shader);


	



	// set up vertex buffers
	// Generate the vertex array
	glGenVertexArrays(1, &opengl->grid_debug_VAO);
	// Bind vertex array to buffer
	glBindVertexArray(opengl->grid_debug_VAO);
	
	// Generate vertex buffer object
	glGenBuffers(1, &opengl->grid_debug_VBO);
	// bind vertex buffer object to buffer
	glBindBuffer(GL_ARRAY_BUFFER, opengl->grid_debug_VBO);
	

	int grid_size = DEBUG_GRID_SIZE; 

	int axis_vertex_count = 6;
	opengl->grid_mesh.vertex_count = (grid_size + 1) * 4 + axis_vertex_count;
	opengl->grid_mesh.pos = cast(Vec3f*)linear_alloc(&opengl->renderer_allocator, opengl->grid_mesh.vertex_count * sizeof(Vec3f), 4);

	opengl->grid_mesh.color = cast(Vec3f*)linear_alloc(&opengl->renderer_allocator, opengl->grid_mesh.vertex_count * sizeof(Vec3f), 4);
	

	
	Vec3f color_white = { 1, 1, 1 };
	int index = 0;
	float half_unit_size = 0.5f;
	for (int i = -grid_size; i <= grid_size; i+=2) {
		float size_f = half_unit_size  * grid_size;
		float i_f = half_unit_size * i;

		Vec3f pt = { -size_f, 0, i_f };
		Vec3f pt2 = { size_f, 0, i_f };

		opengl->grid_mesh.pos[index + 0] = pt;
		opengl->grid_mesh.pos[index + 1] = pt2;


		pt =  make_vec3f( i_f, 0, -size_f);
		pt2 = make_vec3f( i_f, 0, size_f);

		opengl->grid_mesh.pos[index + 2] = pt;
		opengl->grid_mesh.pos[index + 3] = pt2;

		opengl->grid_mesh.color[index + 0] = color_white;
		opengl->grid_mesh.color[index + 1] = color_white;
		opengl->grid_mesh.color[index + 2] = color_white;
		opengl->grid_mesh.color[index + 3] = color_white;

		

		index+=4;

	}

	int axis_offset_pos = index;
	opengl->axes_pos_offset = index;

	float axis_scale = 1.0f;
	Vec3f x_axis = { axis_scale, 0, 0 };
	Vec3f y_axis = { 0, axis_scale, 0 };
	Vec3f z_axis = { 0, 0, axis_scale };
	Vec3f origin = { 0, 0, 0 };
	
	// Pos
	opengl->grid_mesh.pos[axis_offset_pos] = origin;
	opengl->grid_mesh.pos[axis_offset_pos + 1] = x_axis;
	// Color
	opengl->grid_mesh.color[axis_offset_pos] = x_axis;
	opengl->grid_mesh.color[axis_offset_pos + 1] = x_axis;


	// Pos
	opengl->grid_mesh.pos[axis_offset_pos + 2] = origin;
	opengl->grid_mesh.pos[axis_offset_pos + 3] = y_axis;
	// Color
	opengl->grid_mesh.color[axis_offset_pos + 2] = y_axis;
	opengl->grid_mesh.color[axis_offset_pos + 3] = y_axis;

	// Pos
	opengl->grid_mesh.pos[axis_offset_pos + 4] = origin;
	opengl->grid_mesh.pos[axis_offset_pos + 5] = z_axis;
	// Color
	opengl->grid_mesh.color[axis_offset_pos + 4] = z_axis;
	opengl->grid_mesh.color[axis_offset_pos + 5] = z_axis;



}
void destroy_gl_debug(OpenGLRenderer* opengl) {
	

	delete_gl_program(&opengl->debug_shader);
	
	glDeleteVertexArrays(1, &opengl->grid_debug_VAO);
	glDeleteBuffers(1, &opengl->grid_debug_VBO);


}


bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, void* parition_start, size_t partition_size) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	opengl->gl_context = SDL_GL_CreateContext(window);
	opengl->sdl_window = window;
	

	opengl->renderer_memory = parition_start;
	opengl->renderer_memory_size = partition_size;
	linear_init(&opengl->renderer_allocator, opengl->renderer_memory, opengl->renderer_memory_size);

	init_gl_extensions(opengl);	

	load_shaders(opengl);

	init_gl_debug(opengl);


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	return true;
}

bool destroy_opengl_renderer(OpenGLRenderer* opengl) {
	SDL_GL_DeleteContext(opengl->gl_context);
	// TODO: we should have a list of shaders instead of manually having to call it
	delete_gl_program(&opengl->main_shader);
	delete_gl_program(&opengl->debug_shader);
	delete_gl_program(&opengl->equi_rect_shader);
	delete_gl_program(&opengl->shadow_shader);
	

	glDeleteVertexArrays(1, &opengl->VAO);
	glDeleteBuffers(1, &opengl->VBO);
	glDeleteBuffers(1, &opengl->EBO);
	glDeleteFramebuffers(1, &opengl->shadow_fbo);
	
	

	destroy_gl_debug(opengl);
	linear_reset(&opengl->renderer_allocator);
	return true;
}

void opengl_debug_render(OpenGLRenderer* opengl, Vec2i viewport_size) {

	glViewport(0, 0, viewport_size.x, viewport_size.y);


	Camera camera = opengl->render_scene->main_camera;
	Mat4x4f model_mat = mat4x4f_identity();
	Mat4x4f view_mat = camera.view_mat;//look_at(camera.pos, v3_add(camera.pos, camera.forward), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	
	Mat4x4f mvp_mat = mat4x4_mul(&projection_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &model_mat);


	glBindVertexArray(opengl->grid_debug_VAO);
	glUseProgram(opengl->debug_shader.program);

	glUniformMatrix4fv(glGetUniformLocation(opengl->debug_shader.program, "transform"), 1, GL_FALSE, mvp_mat.mat1d);



	// Draw grid
	glBindBuffer(GL_ARRAY_BUFFER, opengl->grid_debug_VBO);


	glBufferData(GL_ARRAY_BUFFER,
		2 * opengl->grid_mesh.vertex_count * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	// Buffer pos data
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->grid_mesh.vertex_count * sizeof(Vec3f),
		opengl->grid_mesh.pos);

	// Buffer color data
	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->grid_mesh.vertex_count * sizeof(Vec3f),
		opengl->grid_mesh.vertex_count * sizeof(Vec3f),
		opengl->grid_mesh.color);


	// Pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(opengl->grid_mesh.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	if (opengl->show_debug_grid) {
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, opengl->axes_pos_offset);
	}

	if (opengl->show_debug_axes) {
		glLineWidth(4);
		glDrawArrays(GL_LINES, opengl->axes_pos_offset, opengl->grid_mesh.vertex_count - opengl->axes_pos_offset);

	}


	// Draw light


	Vec3f dir_light_line[2];
	Vec3f dir_light_color[2];

	dir_light_line[1]  = opengl->render_scene->test_light.direction;
	dir_light_line[0] = make_vec3f(0, 0, 0);
	

	dir_light_color[0] = make_vec3f(1, 1, 1);
	dir_light_color[1] = make_vec3f(1, 1, 1);
	glBufferData(GL_ARRAY_BUFFER,
		4 * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	// Buffer pos data
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		2 * sizeof(Vec3f),
		&dir_light_line);


	Vec3f color = make_vec3f(1, 1, 1);
	// Buffer color data
	glBufferSubData(GL_ARRAY_BUFFER,
		2 * sizeof(Vec3f),
		2 * sizeof(Vec3f),
		&dir_light_color);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(2 * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	glPointSize(25);
	glDrawArrays(GL_LINES, 0, 2);

}



static void opengl_render_scene(OpenGLRenderer* opengl, Vec2i viewport_size, bool light_pass) {
	Camera camera = opengl->render_scene->main_camera;

	int draw_type = GL_TRIANGLES;
	if (opengl->draw_lines) {
		draw_type = GL_LINES;
	} else {
		draw_type = GL_TRIANGLES;
	}



	Mat4x4f view_mat = camera.view_mat;
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f pv_mat;
	// TODO: hoist up the scene var
	if (light_pass) {
		// While a directional light has no position, we treat the direction like a postion, where the direction of the light is the vector to the origin
		view_mat = look_at(opengl->render_scene->test_light.direction, make_vec3f(0, 0, 0), Vec3f_Up);
		projection_mat = ortho(-camera.far, camera.far, viewport_size.y * 0.01f, -viewport_size.y* 0.01f, viewport_size.x* 0.01f, -viewport_size.x* 0.01f);
		pv_mat = mat4x4_mul(&projection_mat, &view_mat);
	
		

		opengl->light_space_mat = pv_mat;
	} else {
		glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
		pv_mat = mat4x4_mul(&projection_mat, &view_mat);
	}


	// TODO: need a better way to handle these as the scene complexity goes us
	Mat4x4f test_mesh_model_mat = trs_mat_from_transform(&opengl->render_scene->mesh_test.transform);
	Mat4x4f test_mesh2_model_mat = trs_mat_from_transform(&opengl->render_scene->mesh_test2.transform);
	Mat4x4f plane_mesh_model_mat = trs_mat_from_transform(&opengl->render_scene->flat_plane.transform);


	


	GLint current_shader = light_pass ? opengl->shadow_shader.program : opengl->main_shader.program;

	// TODO: figure out shader organization. Uber shader?

	// Texture Shader
	glBindVertexArray(opengl->VAO);	

	glUseProgram(current_shader);
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "model"), 1, GL_FALSE, test_mesh_model_mat.mat1d);
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "projection_view"), 1, GL_FALSE, pv_mat.mat1d);

	if (!light_pass) {
		glUniformMatrix4fv(glGetUniformLocation(current_shader, "light_space_mat"), 1, GL_FALSE, opengl->light_space_mat.mat1d);



		GLuint uniform_index = glGetUniformLocation(current_shader, "shadowMap");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
		glUniform1i(uniform_index, 0);


		uniform_index = glGetUniformLocation(current_shader, "albedo_map");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, opengl->albedo_map_id);
		glUniform1i(uniform_index, 1);

		uniform_index = glGetUniformLocation(current_shader, "normal_map");
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, opengl->normal_map_id);
		glUniform1i(uniform_index, 2);


		uniform_index = glGetUniformLocation(current_shader, "metallic_map");
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, opengl->metallic_map_id);
		glUniform1i(uniform_index, 3);

		uniform_index = glGetUniformLocation(current_shader, "roughness_map");
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, opengl->roughness_map_id);
		glUniform1i(uniform_index, 4);

		uniform_index = glGetUniformLocation(current_shader, "ao_map");
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, opengl->ao_map_id);
		glUniform1i(uniform_index, 5);	

		uniform_index = glGetUniformLocation(current_shader, "irradiance_map");
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->irradiance_map_id);
		glUniform1i(uniform_index, 6);
		
		
	}
	
	


	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f)
		+ opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test.pos);


	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f),
		opengl->render_scene->mesh_test.texcoords);

	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f)
		 + opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f),
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test.normal);


	// TODO:  https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	// Should we pre interleave our data?
	// since our pos and texcoords are not interleaved as ptn ptn pt, we need to batch the pos first then texcoords
	

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);

	// Normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f),
		cast(GLvoid*)(opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f) + opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.index_count * sizeof(Vec3i),
		opengl->render_scene->mesh_test.indices,
		GL_STATIC_DRAW);



	glDrawElements(draw_type, 3 * opengl->render_scene->mesh_test.index_count, GL_UNSIGNED_INT, 0);




	glUniformMatrix4fv(glGetUniformLocation(current_shader, "model"), 1, GL_FALSE, test_mesh2_model_mat.mat1d);

	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec2f)
		+ opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test2.pos);


	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec2f),
		opengl->render_scene->mesh_test2.texcoords);

	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec2f),
		opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f),
		opengl->render_scene->mesh_test2.normal);


	// TODO:  https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	// Should we pre interleave our data?
	// since our pos and texcoords are not interleaved as ptn ptn pt, we need to batch the pos first then texcoords


	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texcoords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);

	// Normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f),
		cast(GLvoid*)(opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec3f) + opengl->render_scene->mesh_test2.vertex_count * sizeof(Vec2f)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->mesh_test2.index_count * sizeof(Vec3i),
		opengl->render_scene->mesh_test2.indices,
		GL_STATIC_DRAW);



	glDrawElements(draw_type, 3 * opengl->render_scene->mesh_test2.index_count, GL_UNSIGNED_INT, 0);






	// Simple colored shader
	//glBindVertexArray(opengl->VAO);


	//debug_print("camera pos %f, %f, %f\n", camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
	//debug_print("light pos %f, %f, %f\n", opengl->render_scene->test_light.direction.x, opengl->render_scene->test_light.direction.y, opengl->render_scene->test_light.direction.z);

	glUniform3f(glGetUniformLocation(current_shader, "camera_pos"), camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
	glUniform3f(glGetUniformLocation(current_shader, "light_pos"), opengl->render_scene->test_light.direction.x, opengl->render_scene->test_light.direction.y, opengl->render_scene->test_light.direction.z);
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "model"), 1, GL_FALSE, plane_mesh_model_mat.mat1d);
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "projection_view"), 1, GL_FALSE, pv_mat.mat1d);

	

	// Draw primative test

	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->flat_plane.vertex_count * sizeof(Vec2f)
		+ opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f),
		opengl->render_scene->flat_plane.pos);

	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f),
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec2f),
		opengl->render_scene->flat_plane.texcoords);

	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f) +
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec2f),
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f),
		opengl->render_scene->flat_plane.normal);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.index_count * sizeof(Vec3i),
		opengl->render_scene->flat_plane.indices,
		GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f),
		cast(GLvoid*)(opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f) + opengl->render_scene->flat_plane.vertex_count * sizeof(Vec2f)));
	glEnableVertexAttribArray(2);

	glDrawElements(draw_type, 3 * opengl->render_scene->flat_plane.index_count, GL_UNSIGNED_INT, 0);


	// Skip skybox drawing if a light pass
	if (light_pass) return;
	// Draw skybox

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	glBindVertexArray(opengl->skybox_VAO);
	glUseProgram(opengl->skybox_shader.program);
	glUniformMatrix4fv(glGetUniformLocation(opengl->skybox_shader.program, "projection"), 1, GL_FALSE, projection_mat.mat1d);
	glUniformMatrix4fv(glGetUniformLocation(opengl->skybox_shader.program, "view"), 1, GL_FALSE, view_mat.mat1d);
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->env_cubemap);
	glUniform1i(glGetUniformLocation(opengl->skybox_shader.program, "skybox"), 0);


	glBindBuffer(GL_ARRAY_BUFFER, opengl->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->hdr_skymap.cube.vertex_count * sizeof(Vec3f),
		opengl->render_scene->hdr_skymap.cube.pos,
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->skybox_EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->hdr_skymap.cube.index_count * sizeof(Vec3i),
		opengl->render_scene->hdr_skymap.cube.indices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glFrontFace(GL_CW);

	glDrawElements(GL_TRIANGLES, 3 * opengl->render_scene->hdr_skymap.cube.index_count, GL_UNSIGNED_INT, 0);


	glFrontFace(GL_CCW);

	glDepthFunc(GL_LESS); // set depth function back to default



	

}

void opengl_render(OpenGLRenderer* opengl, Vec2i viewport_size, bool render_debug) {
	
	
	// Shadow pass

	//glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->shadow_fbo);
	glViewport(0, 0, opengl->shadow_width_res, opengl->shadow_height_res);
	glClear(GL_DEPTH_BUFFER_BIT);
	opengl_render_scene(opengl, viewport_size, true);
	////glCullFace(GL_BACK); // don't forget to reset original culling face
	

	// Normal lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewport_size.x, viewport_size.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	opengl_render_scene(opengl, viewport_size, false);
	
	if (render_debug) {
		opengl_debug_render(opengl, viewport_size);
	}
	
	
}




void opengl_swap_buffer(OpenGLRenderer* opengl) {
	SDL_GL_SwapWindow(opengl->sdl_window);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
