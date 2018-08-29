#pragma once


#include "OpenGLRenderer.h"
#include "../../debug_macros.h"
#include "../../Common/common_macros.h"

#include "../../utils.c"


static void init_skybox(OpenGLRenderer* opengl, Scene* scene) {
	// Bind skybox
	glGenTextures(1, &opengl->skybox_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->skybox_id);
	int width = scene->skybox.front.width;
	int height = scene->skybox.front.height;

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.right.data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.left.data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.top.data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.bottom.data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.back.data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->skybox.front.data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	

	glGenVertexArrays(1, &opengl->skybox_VAO);
	glGenBuffers(1, &opengl->skybox_VBO);
	glBindVertexArray(opengl->skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, scene->skybox.cube.vertex_count * sizeof(Vec3f), scene->skybox.cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	
	

	const char* vertex_shader = file_to_str("Assets/shaders/skybox.vs", &opengl->renderer_allocator);
	const char* fragment_shader = file_to_str("Assets/shaders/skybox.fs", &opengl->renderer_allocator);

	load_gl_shader(&opengl->skybox_shader, &vertex_shader, &fragment_shader);
}






void set_scene_for_opengl_renderer(OpenGLRenderer* opengl, Scene* scene) {
	// TODO: instead of passing the entire scene, we should pass a culled scene for faster rendering
	opengl->render_scene = scene;

	init_skybox(opengl, scene);



	opengl->shadow_width_res = SHADOW_WIDTH_RES;
	opengl->shadow_height_res = SHADOW_HEIGHT_RES;
	

	glGenFramebuffers(1, &opengl->shadow_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->shadow_fbo);

	glGenTextures(1, &opengl->shadow_map);
	glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, opengl->shadow_width_res, opengl->shadow_height_res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, opengl->shadow_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		debug_print("Frame buffer not created");
	}
		

	


	// Bind Model textures
	glGenTextures(1, &opengl->textureID);
	glBindTexture(GL_TEXTURE_2D, opengl->textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene->texture_test.width, scene->texture_test.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene->texture_test.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(opengl->textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(opengl->textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



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
	vertex_shader = file_to_str("Assets/shaders/simple.vs", &opengl->renderer_allocator);
	fragment_shader = file_to_str("Assets/shaders/simple.fs", &opengl->renderer_allocator);
	load_gl_shader(&opengl->simple_shader, &vertex_shader, &fragment_shader);
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
	delete_gl_program(&opengl->skybox_shader);
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

	glBufferData(GL_ARRAY_BUFFER,
		2 * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	// Buffer pos data
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		sizeof(Vec3f),
		&opengl->render_scene->test_light.transform.position);


	Vec3f color = make_vec3f(1, 1, 1);
	// Buffer color data
	glBufferSubData(GL_ARRAY_BUFFER,
		sizeof(Vec3f),
		sizeof(Vec3f),
		&color);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	glPointSize(25);
	glDrawArrays(GL_POINTS, 0, 1);

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
		view_mat = look_at(opengl->render_scene->test_light.transform.position , make_vec3f(0, 0, 0), Vec3f_Up);
		projection_mat = ortho(-camera.far, camera.far, viewport_size.y * 0.01f, -viewport_size.y* 0.01f, viewport_size.x* 0.01f, -viewport_size.x* 0.01f);
		pv_mat = mat4x4_mul(&projection_mat, &view_mat);
		opengl->light_space_mat = pv_mat;
	} else {
		pv_mat = mat4x4_mul(&projection_mat, &view_mat);
	}


	// TODO: need a better way to handle these as the scene complexity goes us
	Mat4x4f test_mesh_model_mat = trs_mat_from_transform(&opengl->render_scene->mesh_test.transform);
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


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opengl->textureID);
		glUniform1i(glGetUniformLocation(current_shader, "textureSampler"), 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
		glUniform1i(glGetUniformLocation(current_shader, "shadowMap"), 1);
	}
	
	


	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f) + opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f),
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


	// TODO:  https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	// Should we pre interleave our data?
	// since our pos and texcoords are not interleaved as pt pt pt, we need to batch the pos first then texcoords
	// our mesh is laid out like ppp ttt
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.index_count * sizeof(Vec3i),
		opengl->render_scene->mesh_test.indices,
		GL_STATIC_DRAW);



	glDrawElements(draw_type, 3 * opengl->render_scene->mesh_test.index_count, GL_UNSIGNED_INT, 0);







	// Simple colored shader
	glBindVertexArray(opengl->VAO);

	current_shader = light_pass ? opengl->shadow_shader.program : opengl->main_shader.program;
	glUseProgram(current_shader);


	glUniformMatrix4fv(glGetUniformLocation(current_shader, "model"), 1, GL_FALSE, plane_mesh_model_mat.mat1d);
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "projection_view"), 1, GL_FALSE, pv_mat.mat1d);

	//if (!light_pass) {
	//	glUniformMatrix4fv(glGetUniformLocation(current_shader, "light_space_mat"), 1, GL_FALSE, opengl->light_space_mat.mat1d);


	//	glUniform1i(glGetUniformLocation(current_shader, "textureSampler"), 0);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, opengl->textureID);


	//	glUniform1i(glGetUniformLocation(current_shader, "shadowMap"), 0);
	//	glActiveTexture(GL_TEXTURE1);
	//	glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
	//	
	//}

	// Draw primative test

	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->flat_plane.vertex_count * sizeof(Vec2f),
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->flat_plane.index_count * sizeof(Vec3i),
		opengl->render_scene->flat_plane.indices,
		GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(opengl->render_scene->flat_plane.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);



	glLineWidth(1.0f);
	glDrawElements(draw_type, 3 * opengl->render_scene->flat_plane.index_count, GL_UNSIGNED_INT, 0);


	// Skip skybox drawing if a light pass
	if (light_pass) return;
	// Draw skybox

	Mat4x4f skybox_mat = mat4x4f_identity();
	Mat4x4f skybox_transform = mat4x4_mul(&pv_mat, &skybox_mat);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	glUseProgram(opengl->skybox_shader.program);
	glUniformMatrix4fv(glGetUniformLocation(opengl->skybox_shader.program, "transform"), 1, GL_FALSE, skybox_transform.mat1d);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->skybox_id);





	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->skybox.cube.vertex_count * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->render_scene->skybox.cube.vertex_count * sizeof(Vec3f),
		opengl->render_scene->skybox.cube.pos);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_scene->skybox.cube.index_count * sizeof(Vec3i),
		opengl->render_scene->skybox.cube.indices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glFrontFace(GL_CW);

	glDrawElements(GL_TRIANGLES, 3 * opengl->render_scene->skybox.cube.index_count, GL_UNSIGNED_INT, 0);


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
	//glCullFace(GL_BACK); // don't forget to reset original culling face
	

	// Normal lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewport_size.x, viewport_size.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, opengl->shadow_map);
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
