#pragma once


#include "OpenGLRenderer.h"
#include "../../debug_macros.h"
#include "../../Common/common_macros.h"


const char * vertex_shader = "#version 330 core\n\
		layout (location = 0) in vec3 position;\
		layout (location = 1) in vec2 uv;\
		uniform mat4 transform;\
		out vec2 frag_uv;\
		void main(){\
			gl_Position =   transform * vec4(position, 1); \
			frag_uv = uv;\
		}";

const char * fragment_shader = "#version 330 core\n\
		in vec2 frag_uv;\
		out vec4 color;\
		uniform sampler2D txtSampler;\
		void main(){\
			color = texture(txtSampler, frag_uv).rgba;\
		}";


const char * debug_vertex_shader = "#version 330 core\n\
		layout (location = 0) in vec3 position;\
		layout (location = 1) in vec3 color;\
		uniform mat4 transform;\
		out vec4 vertexColor;\
		void main(){\
			gl_Position = transform * vec4(position, 1);\
			vertexColor = vec4(color, 1);\
		}";

const char * debug_fragment_shader = "#version 330 core\n\
		in vec4 vertexColor;\
		out vec4 color;\
		void main(){\
			color = vertexColor;\
		}";



void set_scene_for_opengl_renderer(OpenGLRenderer* opengl, Scene* scene) {
	// TODO: instead of passing the entire scene, we should pass a culled scene for faster rendering
	opengl->render_scene = scene;


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
	load_gl_shader(&opengl->main_shader, &vertex_shader, &fragment_shader);
}


void init_gl_debug(OpenGLRenderer* opengl) {
	opengl->show_debug_grid = false;
	opengl->show_debug_axes = false;
	


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
	for (int i = -grid_size; i <= grid_size; i+=2) {
		float size_f = 1.0f * grid_size;
		float i_f = 1.0f * i;

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
	delete_gl_program(&opengl->main_shader);
	

	glDeleteVertexArrays(1, &opengl->VAO);
	glDeleteBuffers(1, &opengl->VBO);
	glDeleteBuffers(1, &opengl->EBO);
	
	

	destroy_gl_debug(opengl);
	linear_reset(&opengl->renderer_allocator);
	return true;
}



void opengl_render(OpenGLRenderer* opengl, Vec2i viewport_size) {
	
	
	glViewport(0, 0, viewport_size.x, viewport_size.y);

	
	Camera camera = opengl->render_scene->main_camera;
	//Mat4x4f model_mat = mat4x4f_identity();
	
	Mat4x4f model_mat = rotate(deg_to_rad(camera.rotation.y), Vec3f_Up);
	Mat4x4f rot2 = rotate(deg_to_rad(camera.rotation.z), Vec3f_Forward);
	model_mat = mat4x4_mul(&model_mat, &rot2);
	
	

	Mat4x4f view_mat = look_at(camera.pos.xyz, v3_add(camera.pos.xyz, camera.dir.xyz), Vec3f_Up);
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	Mat4x4f mvp_mat = mat4x4_mul(&projection_mat, &view_mat);
	mvp_mat = mat4x4_mul(&mvp_mat, &model_mat);



	glBindVertexArray(opengl->VAO);
	glUseProgram(opengl->main_shader.program);

	glUniformMatrix4fv(glGetUniformLocation(opengl->main_shader.program, "transform"), 1, GL_FALSE, mvp_mat.mat1d);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengl->textureID);
	
	glUniform1i(glGetUniformLocation(opengl->main_shader.program, "txtSampler"), 0);

	
	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_scene->mesh_test.vertex_count * sizeof(Vec3f)
		+ opengl->render_scene->mesh_test.vertex_count * sizeof(Vec2f),
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

	
	
	glDrawElements(GL_TRIANGLES, 3 * opengl->render_scene->mesh_test.index_count, GL_UNSIGNED_INT, 0);

	
	
}

void opengl_debug_render(OpenGLRenderer* opengl, Vec2i viewport_size) {

	glViewport(0, 0, viewport_size.x, viewport_size.y);

	
	Camera camera = opengl->render_scene->main_camera;
	//Mat4x4f model_mat = mat4x4f_identity();

	Mat4x4f model_mat = rotate(deg_to_rad(camera.rotation.y), Vec3f_Up);
	Mat4x4f rot2 = rotate(deg_to_rad(camera.rotation.z), Vec3f_Forward);
	model_mat = mat4x4_mul(&model_mat, &rot2);


	Mat4x4f view_mat = look_at(camera.pos.xyz, v3_add(camera.pos.xyz, camera.dir.xyz), Vec3f_Up);
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
	
}



void opengl_swap_buffer(OpenGLRenderer* opengl) {
	SDL_GL_SwapWindow(opengl->sdl_window);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
