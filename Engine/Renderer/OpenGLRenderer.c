#pragma once


#include "OpenGLRenderer.h"





const char * vertex_shader = "#version 330 core\n\
		layout (location = 0) in vec3 position;\n\
		layout (location = 1) in vec2 uv;\
		out vec2 frag_uv;\
		void main(){\
			gl_Position = vec4(position,1);\
			frag_uv = uv;\
		}";

const char * fragment_shader = "#version 330 core\n\
		in vec2 frag_uv;\
		out vec4 color;\
		uniform sampler2D txtSampler;\
		void main(){\
			color = texture(txtSampler, frag_uv).rgba;\
		}";

static void init_gl_extensions(OpenGLRenderer* opengl) {
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		printf("Failed to init GLEW\n");
	} else {
		printf("Succeeded to init GLEW\n");
	}
}

static void load_shaders(OpenGLRenderer* opengl) {
 	opengl->vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(opengl->vs, 1, &vertex_shader, NULL);
	glCompileShader(opengl->vs);


	GLint success;
	GLchar infoLog[512];

	// Print compile errors if any
	glGetShaderiv(opengl->vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(opengl->vs, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED, %s\n", infoLog);
	};


	opengl->fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(opengl->fs, 1, &fragment_shader, NULL);
	glCompileShader(opengl->fs);
	// Print compile errors if any
	glGetShaderiv(opengl->fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(opengl->fs, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED, %s\n", infoLog);
	};

	// Shader Program
	opengl->shader_program = glCreateProgram();
	glAttachShader(opengl->shader_program, opengl->vs);
	glAttachShader(opengl->shader_program, opengl->fs);
	glLinkProgram(opengl->shader_program);
	// Print linking errors if any

	glGetProgramiv(opengl->shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(opengl->shader_program, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED, %s\n", infoLog);
	}

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(opengl->vs);
	glDeleteShader(opengl->fs);
}


bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, Vec2i window_size) {
	opengl->gl_context = SDL_GL_CreateContext(window);
	opengl->sdl_window = window;
	opengl->window_size = window_size;
	init_gl_extensions(opengl);

	load_shaders(opengl);


	/*load_obj("Assets/obj/diablo3_pose.obj", &opengl->model);
	load_image("Assets/obj/diablo3_pose_diffuse.tga", &opengl->model.diffuse);*/

	ObjModel model;

	load_obj("Assets/obj/african_head.obj", &model);
	load_image("Assets/obj/african_head_diffuse.tga", &opengl->texture);
	convert_to_static_mesh(&model, &opengl->mesh);

	free_obj(&model);

	


	
	
	

	
	
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);


	glGenTextures(1, &opengl->textureID);

	
	glBindTexture(GL_TEXTURE_2D, opengl->textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl->texture.surface->w, opengl->texture.surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl->texture.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Texture parameters
	
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

	


	
	
	// TODO:  https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
	// Should we pre interleave our data, or create sperate vbos for textures.
	// Also note, since our textcoords array is not offset from our vert array. 2 seperate malloc calls, so different addresses


	
	return true;
}

bool destroy_opengl_renderer(OpenGLRenderer* opengl) {
	SDL_GL_DeleteContext(opengl->gl_context);
	glDeleteProgram(opengl->shader_program);
	
	
	free_texture(&opengl->texture);
	free_static_mesh(&opengl->mesh);
	return true;
}

void opengl_render(OpenGLRenderer* opengl) {
	
	

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindVertexArray(opengl->VAO);
	glUseProgram(opengl->shader_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengl->textureID);
	


	glUniform1i(glGetUniformLocation(opengl->shader_program, "txtSampler"), 0);


	glBufferData(GL_ARRAY_BUFFER,
		opengl->mesh.vertex_count * sizeof(Vec3f)
		+ opengl->mesh.vertex_count * sizeof(Vec2f),
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		opengl->mesh.vertex_count * sizeof(Vec3f),
		opengl->mesh.pos);


	glBufferSubData(GL_ARRAY_BUFFER,
		opengl->mesh.vertex_count * sizeof(Vec3f),
		opengl->mesh.vertex_count * sizeof(Vec2f),
		opengl->mesh.texcoords);



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (GLvoid*)(opengl->mesh.vertex_count * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		 opengl->mesh.index_count * sizeof(Vec3i),
		opengl->mesh.indices,
		GL_STATIC_DRAW);

	
	
	glDrawElements(GL_TRIANGLES, 3 * opengl->mesh.index_count, GL_UNSIGNED_INT, 0);
	
	

	
	SDL_GL_SwapWindow(opengl->sdl_window);
}