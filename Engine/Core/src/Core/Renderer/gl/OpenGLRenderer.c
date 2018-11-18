#pragma once


#include "Core/Renderer/gl/OpenGLRenderer.h"

#include "debug_macros.h"
#include "Common/common_macros.h"
#include "Math/Math.h"
#include "Util/Utils.h"


#define uniform3f_pack(vec) vec.x, vec.y, vec.z

void gl_init_hdr_map(OpenGLRenderer* opengl, HDR_SkyMap* skymap) {
	
	// IBL, and pbr code taken from https://learnopengl.com/PBR/IBL/Specular-IBL
	// Code is very similar but slightly modified to our engine
	// For now, we'll start our renderer using this code, but eventually move to something more custom and flexible
	// Example, reflection probes
	// Precompute irradiance maps, and other maps
	int skymap_buffer_width = 512;
	int skymap_buffer_height = 512;

	
	// TODO: hoist up all the opengl->render_world->xxxxx vars here
	opengl->render_world->capture_fbo_res = gl_create_fbo(opengl);
	opengl->render_world->capture_rbo_res = gl_create_rbo(opengl, skymap_buffer_width, skymap_buffer_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, opengl->rbos[opengl->render_world->capture_rbo_res.handle]);


	opengl->render_world->hdr_skymap_res = gl_create_hdr_texture(opengl, &skymap->map, GL_RGB, GL_RGB16F);
	opengl->render_world->env_cubemap_res = gl_create_cubemap(opengl, skymap_buffer_width, skymap_buffer_height, false);

	// Load skymap to equirectangular shader 
	opengl->render_world->equirectangular_shader_res = gl_create_shader(opengl, "Assets/shaders/hdr_skymap.vs", "Assets/shaders/hdr_skymap.fs");
	opengl->render_world->irradiance_shader_res = gl_create_shader(opengl, "Assets/shaders/hdr_skymap.vs", "Assets/shaders/irradiance_conv.fs");
	opengl->render_world->prefilter_shader_res = gl_create_shader(opengl, "Assets/shaders/hdr_skymap.vs", "Assets/shaders/prefilter.fs");
	opengl->render_world->brdf_shader_res = gl_create_shader(opengl, "Assets/shaders/brdf.vs", "Assets/shaders/brdf.fs");
	opengl->render_world->skybox_shader_res = gl_create_shader(opengl, "Assets/shaders/skybox.vs", "Assets/shaders/skybox.fs");
	


	

	

	Mat4x4f captureProjection = perspective(0.1f, 10.0f, 90.0f, 1.0f);
	
	Vec3f center = Vec3f(0, 0, 0);
	Mat4x4f captureViews[] =
	{
		look_at(center, Vec3f(1.0f,  0.0f,  0.0f), Vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, Vec3f(-1.0f,  0.0f,  0.0f),Vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, Vec3f(0.0f,  1.0f,  0.0f), Vec3f(0.0f,  0.0f,  1.0f)),
		look_at(center, Vec3f(0.0f, -1.0f,  0.0f), Vec3f(0.0f,  0.0f, -1.0f)),
		look_at(center, Vec3f(0.0f,  0.0f,  1.0f), Vec3f(0.0f, -1.0f,  0.0f)),
		look_at(center, Vec3f(0.0f,  0.0f, -1.0f), Vec3f(0.0f, -1.0f,  0.0f))
	};



	opengl->render_world->skybox_vao_res = gl_create_vao(opengl);
	opengl->render_world->skybox_vbo_res = gl_create_vbo(opengl);
	opengl->render_world->skybox_ebo_res = gl_create_ebo(opengl);
	
	
	
	
	

	glBindVertexArray(opengl->vaos[opengl->render_world->skybox_vao_res.handle]);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->skybox_vbo_res.handle]);
	glBufferData(GL_ARRAY_BUFFER, skymap->cube.vertex_count * sizeof(Vec3f), skymap->cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	

	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate env map from equirectangular map

	GLShader equi_rect_shader = opengl->shaders[opengl->render_world->equirectangular_shader_res.handle];
	
	glUseProgram(equi_rect_shader.program);
	glUniformMatrix4fv(glGetUniformLocation(equi_rect_shader.program, "projection"), 1, GL_FALSE, captureProjection.mat1d);

	glUniform1i(glGetUniformLocation(equi_rect_shader.program, "equirectangular_map"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->hdr_skymap_res.handle]);

	

	
	glViewport(0, 0, skymap_buffer_width, skymap_buffer_height); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->capture_fbo_res.handle]);

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glFrontFace(GL_CW);

	GLint i_uni = glGetUniformLocation(equi_rect_shader.program, "view");
	
	for (unsigned int i = 0; i < 6; ++i) {
	
	
		glUniformMatrix4fv(i_uni, 1, GL_FALSE, captureViews[i].mat1d);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, opengl->textures[opengl->render_world->env_cubemap_res.handle], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->ebos[opengl->render_world->skybox_ebo_res.handle]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			skymap->cube.index_count * sizeof(Vec3i),
			skymap->cube.indices,
			GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 3 * skymap->cube.index_count, GL_UNSIGNED_INT, 0);
	}
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS); // set depth function back to default


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate irridiance map

	int conv_width = 32;
	int conv_height = 32;

	opengl->render_world->irradiance_map_res = gl_create_cubemap(opengl, conv_width, conv_width, false);



	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->capture_fbo_res.handle]);
	glBindRenderbuffer(GL_RENDERBUFFER, opengl->rbos[opengl->render_world->capture_rbo_res.handle]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, conv_width, conv_height);

	
	
	GLShader irradiance_conv_shader = opengl->shaders[opengl->render_world->irradiance_shader_res.handle];
		
	glUseProgram(irradiance_conv_shader.program);
	
	glUniformMatrix4fv(glGetUniformLocation(irradiance_conv_shader.program, "projection"), 1, GL_FALSE, captureProjection.mat1d);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->textures[opengl->render_world->env_cubemap_res.handle]);
	glUniform1i(glGetUniformLocation(irradiance_conv_shader.program, "environment_map"), 0);
	
	
	glBindVertexArray(opengl->vaos[opengl->render_world->skybox_vao_res.handle]);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->skybox_vbo_res.handle]);
	glBufferData(GL_ARRAY_BUFFER, skymap->cube.vertex_count * sizeof(Vec3f), skymap->cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glFrontFace(GL_CW);

	glViewport(0, 0, conv_width, conv_height);
	
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->capture_fbo_res.handle]);
	i_uni = glGetUniformLocation(irradiance_conv_shader.program, "view");
	
		
	for (unsigned int i = 0; i < 6; ++i) {
		
		glUniformMatrix4fv(i_uni, 1, GL_FALSE, captureViews[i].mat1d);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, opengl->textures[opengl->render_world->irradiance_map_res.handle], 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->ebos[opengl->render_world->skybox_ebo_res.handle]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			skymap->cube.index_count * sizeof(Vec3i),
			skymap->cube.indices,
			GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 3 * skymap->cube.index_count, GL_UNSIGNED_INT, 0);
	}
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS); // set depth function back to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////////

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	opengl->render_world->prefiler_map_res = gl_create_cubemap(opengl, 128, 128, true);
	
	
	GLShader prefilter_shader = opengl->shaders[opengl->render_world->prefilter_shader_res.handle];
	
	

	

	glUseProgram(prefilter_shader.program);

	glUniformMatrix4fv(glGetUniformLocation(prefilter_shader.program, "projection"), 1, GL_FALSE, captureProjection.mat1d);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->textures[opengl->render_world->env_cubemap_res.handle]);
	glUniform1i(glGetUniformLocation(prefilter_shader.program, "environment_map"), 0);

	
	glBindVertexArray(opengl->vaos[opengl->render_world->skybox_vao_res.handle]);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->skybox_vbo_res.handle]);
	glBufferData(GL_ARRAY_BUFFER, skymap->cube.vertex_count * sizeof(Vec3f), skymap->cube.pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);


	
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glFrontFace(GL_CW);

	
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->capture_fbo_res.handle]);
	// Note maxmip must be at most 5, or else our mipwidth/height, shifting breaks down
	unsigned int maxMipLevels = 5;
	
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
		// reisze framebuffer according to mip-level size.

		//unsigned int mipWidth = (unsigned int) 128 * powf_(0.5, mip);
		//unsigned int mipHeight = (unsigned int)  128 * powf_(0.5, mip);
		unsigned int mipWidth = 2 << ((maxMipLevels + 1) - mip);
		unsigned int mipHeight = 2 << ((maxMipLevels + 1) - mip);
		glBindRenderbuffer(GL_RENDERBUFFER, opengl->rbos[opengl->render_world->capture_rbo_res.handle]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(glGetUniformLocation(prefilter_shader.program, "roughness"), roughness);
		
		i_uni = glGetUniformLocation(prefilter_shader.program, "view");
		
		for (unsigned int i = 0; i < 6; ++i) {
			glUniformMatrix4fv(i_uni, 1, GL_FALSE, captureViews[i].mat1d);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, opengl->textures[opengl->render_world->prefiler_map_res.handle], mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->ebos[opengl->render_world->skybox_ebo_res.handle]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				skymap->cube.index_count * sizeof(Vec3i),
				skymap->cube.indices,
				GL_STATIC_DRAW);

			glDrawElements(GL_TRIANGLES, 3 * skymap->cube.index_count, GL_UNSIGNED_INT, 0);
			
		}
	}
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS); // set depth function back to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////



	/////////////////////////////////////////////////////////////////////////////////////////////////

	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	HDRTexture null_map;
	null_map.width = 512;
	null_map.height = 512;
	null_map.data = 0;
	opengl->render_world->brdf_lut_res = gl_create_hdr_texture(opengl, &null_map, GL_RG, GL_RG16F);
		
	
	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->capture_fbo_res.handle]);
	glBindRenderbuffer(GL_RENDERBUFFER, opengl->rbos[opengl->render_world->capture_rbo_res.handle]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opengl->textures[opengl->render_world->brdf_lut_res.handle], 0);

	glViewport(0, 0, 512, 512);
	glUseProgram(opengl->shaders[opengl->render_world->brdf_shader_res.handle].program);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	

	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	RenderResource quad_vao_res = gl_create_vao(opengl);
	RenderResource quad_vbo_res = gl_create_vbo(opengl);
	
	glBindVertexArray(opengl->vaos[quad_vao_res.handle]);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[quad_vbo_res.handle]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glBindVertexArray(opengl->vaos[quad_vao_res.handle]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	


}

void gl_init_shadow_maps(OpenGLRenderer* opengl) {
	
	opengl->render_world->shadow_shader_res = gl_create_shader(opengl, "Assets/shaders/shadows.vs", "Assets/shaders/shadows.fs");
	opengl->render_world->shadow_fbo_res = gl_create_fbo(opengl);
	opengl->render_world->shadow_map_res = gl_create_shadow_map(opengl, SHADOW_WIDTH_RES, SHADOW_HEIGHT_RES);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, opengl->textures[opengl->render_world->shadow_map_res.handle], 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		debug_print("Frame buffer not created");
	}
}



RenderResource gl_create_texture(OpenGLRenderer* opengl, SimpleTexture* texture, bool mipmap) {
	RenderResource handle;
	handle.type = RenderResourceType_TEXTURE;

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (mipmap) glGenerateMipmap(GL_TEXTURE_2D);

	int next_texture_index = opengl->texture_count;
	handle.handle = next_texture_index;
	opengl->textures[opengl->texture_count] = texture_id;
	opengl->texture_count++;

	
	return handle;
}

RenderResource gl_create_shadow_map(OpenGLRenderer* opengl, unsigned int width, unsigned int height) {
	RenderResource handle;
	handle.type = RenderResourceType_TEXTURE;

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	
	int next_texture_index = opengl->texture_count;
	handle.handle = next_texture_index;
	opengl->textures[opengl->texture_count] = texture_id;
	opengl->texture_count++;

	
	return handle;
}

RenderResource gl_create_hdr_texture(OpenGLRenderer* opengl, HDRTexture* hdr_texture, GLenum format, GLint internalformat) {


	RenderResource handle;
	handle.type = RenderResourceType_TEXTURE;

	GLuint texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	int width = hdr_texture->width;
	int height = hdr_texture->height;
	
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_FLOAT, hdr_texture->data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int next_texture_index = opengl->texture_count;
	handle.handle = next_texture_index;
	opengl->textures[opengl->texture_count] = texture_id;
	opengl->texture_count++;

	

	return handle;
}

RenderResource gl_create_cubemap(OpenGLRenderer* opengl, unsigned int width, unsigned int height, bool mipmap) {
	RenderResource  handle;
	handle.type = RenderResourceType_TEXTURE;

	GLuint texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	for (unsigned int i = 0; i < 6; ++i) {
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR: GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (mipmap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


	int next_texture_index = opengl->texture_count;
	handle.handle = next_texture_index;
	opengl->textures[opengl->texture_count] = texture_id;
	opengl->texture_count++;

	
	return handle;
}

RenderResource gl_create_shader(OpenGLRenderer* opengl, const char* vertex_file, const char* fragment_file) {
	RenderResource handle;
	handle.type = RenderResourceType_SHADER;
	GLShader shader;

	// Alloc on stack
	const char* vertex_shader = file_to_str(vertex_file, &opengl->stack_allocator);
	const char* fragment_shader = file_to_str(fragment_file, &opengl->stack_allocator);
	assert(vertex_shader != NULL);
	assert(fragment_shader != NULL);

	if (vertex_shader && fragment_shader) {
		load_gl_shader(&shader, &vertex_shader, &fragment_shader);
		// Free stack
		stack_pop(&opengl->stack_allocator);
		stack_pop(&opengl->stack_allocator);


		int next_shader_index = opengl->shader_count;
		handle.handle = next_shader_index;
		opengl->shaders[opengl->shader_count] = shader;
		opengl->shader_count++;

		

		return handle;
	} else {
		// TODO: handle case when shader could not be loaded
		debug_print("Shaders %s or %s could not be loaded\n", vertex_file, fragment_file);
		handle.handle = -1;
		return handle;
	}

	
}

RenderResource gl_create_vbo(OpenGLRenderer* opengl) {
	RenderResource handle;
	handle.type = RenderResourceType_VERTEX_BUFFER;
	GLuint vbo_id;

	// Generate vertex buffer object
	glGenBuffers(1, &vbo_id);
	// bind vertex buffer object to buffer to init
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);


	
	int next_vbo_index = opengl->vbo_count;
	handle.handle = next_vbo_index;
	opengl->vbos[opengl->vbo_count] = vbo_id;
	opengl->vbo_count++;

	
	return handle;

}

RenderResource gl_create_ebo(OpenGLRenderer* opengl) {
	RenderResource handle;
	handle.type = RenderResourceType_INDEX_BUFFER;
	GLuint ebo_id;

	// Generate vertex buffer object
	glGenBuffers(1, &ebo_id);
	// bind vertex buffer object to buffer to init
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);


	
	int next_ebo_index = opengl->ebo_count;
	handle.handle = next_ebo_index;
	opengl->ebos[opengl->ebo_count] = ebo_id;
	opengl->ebo_count++;

	
	return handle;

}

RenderResource gl_create_vao(OpenGLRenderer* opengl) {
	RenderResource handle;
	handle.type = RenderResourceType_VERTEX_DECLARATION;
	GLuint vao_id;

	// Generate the vertex array
	glGenVertexArrays(1, &vao_id);
	// Bind vertex array to buffer
	glBindVertexArray(vao_id);


	
	int next_vao_index = opengl->vao_count;
	handle.handle = next_vao_index;
	opengl->vaos[opengl->vao_count] = vao_id;
	opengl->vao_count++;

	
	return handle;

}

RenderResource gl_create_fbo(OpenGLRenderer* opengl) {
	RenderResource handle;
	handle.type = RenderResourceType_FRAME_BUFFER;
	GLuint fbo_id;


	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	//glGenRenderbuffers(1, &rbo_id);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_id);


	


	
	int next_fbo_index = opengl->fbo_count;
	handle.handle = next_fbo_index;
	opengl->fbos[opengl->fbo_count] = fbo_id;
	opengl->fbo_count++;

	
	return handle;
}

RenderResource gl_create_rbo(OpenGLRenderer* opengl, uint32_t width, uint32_t height) {
	RenderResource handle;
	handle.type = RenderResourceType_RENDER_TARGET;
	
	GLuint rbo_id;


	
	glGenRenderbuffers(1, &rbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);




	
	int next_rbo_index = opengl->rbo_count;
	handle.handle = next_rbo_index;
	opengl->rbos[opengl->rbo_count] = rbo_id;
	opengl->rbo_count++;


	return handle;
}
static void init_gl_extensions(OpenGLRenderer* opengl) {
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		debug_print("Failed to init GLEW\n");
	} else {
		debug_print("Succeeded to init GLEW\n");
	}
}

void gizmo_render_axis(OpenGLRenderer* opengl, Vec3f pos, Vec3f forward, Vec3f up, Vec3f right) {


	Camera camera = *opengl->render_world->camera;
	Mat4x4f model_mat;
	Mat4x4f view_mat = camera.view_mat;
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);

	Mat4x4f mvp_mat = projection_mat * view_mat * model_mat;


	GLShader debug_shader = opengl->shaders[opengl->render_world->debug_shader_res.handle];

	glBindVertexArray(opengl->vaos[opengl->render_world->debug_grid_vao_res.handle]);
	glUseProgram(debug_shader.program);

	glUniformMatrix4fv(glGetUniformLocation(debug_shader.program, "transform"), 1, GL_FALSE, mvp_mat.mat1d);
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->debug_grid_vbo_res.handle]);

	
	Vec3f pos_array[6];

	pos_array[0] = pos;
	pos_array[1] = pos + normalize(right);
	pos_array[2] = pos;
	pos_array[3] = pos + normalize(up);
	pos_array[4] = pos;
	pos_array[5] = pos + normalize(forward);

	Vec3f color_array[6]; 
	color_array[0] = Vec3f(1,0,0);
	color_array[1] = Vec3f(1,0,0);
	color_array[2] = Vec3f(0,1,0);
	color_array[3] = Vec3f(0,1,0);
	color_array[4] = Vec3f(0,0,1);
	color_array[5] = Vec3f(0,0,1);



	glBufferData(GL_ARRAY_BUFFER,
		2 * 6 * sizeof(Vec3f),
		NULL,
		GL_STATIC_DRAW);

	// Buffer pos data
	glBufferSubData(GL_ARRAY_BUFFER,
		0,
		6 * sizeof(Vec3f),
		&pos_array);

	// Buffer color data
	glBufferSubData(GL_ARRAY_BUFFER,
		6 * sizeof(Vec3f),
		6 * sizeof(Vec3f),
		&color_array);


	// Pos attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(6 * sizeof(Vec3f)));
	glEnableVertexAttribArray(1);


	
	glLineWidth(4);
	glDrawArrays(GL_LINES, 0, 6);
	

}


bool init_opengl_renderer(SDL_Window* window, OpenGLRenderer* opengl, RenderWorld* render_world) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	opengl->gl_context = SDL_GL_CreateContext(window);
	opengl->sdl_window = window;
	opengl->render_world = render_world;
	
	arena_init(&opengl->arena);

	size_t renderer_mem_size = RENDERER_MEMORY;
	void* renderer_mem_block = arena_alloc(&opengl->arena, renderer_mem_size);
	renderer_mem_size = opengl->arena.end - cast(char*) renderer_mem_block;

	
	stack_alloc_init(&opengl->stack_allocator, renderer_mem_block, renderer_mem_size);

	init_gl_extensions(opengl);
	init_gl_resource_arrays(opengl);

	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// TODO: remove these hardcoded shaders out of here, and instead into the scene loading
	opengl->render_world->texture_shader_res = gl_create_shader(opengl, "Assets/shaders/textured.vs", "Assets/shaders/textured.fs");
	
	opengl->render_world->render_mesh_capacity = 10;
	opengl->render_world->render_mesh_count = 0;
	opengl->render_world->render_mesh_list =  cast(RenderMesh*) stack_alloc(&opengl->stack_allocator, opengl->render_world->render_mesh_capacity * sizeof(RenderMesh), 1);


	opengl->render_world->VAO = gl_create_vao(opengl);
	opengl->render_world->EBO = gl_create_ebo(opengl);
	opengl->render_world->VBO = gl_create_vbo(opengl);
	
	// Debug init
	opengl->show_debug_grid = true;
	opengl->show_debug_axes = true;
	opengl->draw_lines = false;

	opengl->render_world->debug_shader_res = gl_create_shader(opengl, "Assets/shaders/debug.vs", "Assets/shaders/debug.fs");
	opengl->render_world->debug_grid_vao_res = gl_create_vao(opengl);
	opengl->render_world->debug_grid_vbo_res = gl_create_vbo(opengl);


	


	int grid_size = DEBUG_GRID_SIZE;

	int axis_vertex_count = 6;
	opengl->grid_mesh.vertex_count = (grid_size + 1) * 4 + axis_vertex_count;
	opengl->grid_mesh.pos = cast(Vec3f*)stack_alloc(&opengl->stack_allocator, opengl->grid_mesh.vertex_count * sizeof(Vec3f), 4);
	opengl->grid_mesh.color = cast(Vec3f*)stack_alloc(&opengl->stack_allocator, opengl->grid_mesh.vertex_count * sizeof(Vec3f), 4);



	Vec3f color_white = { 1, 1, 1 };
	int index = 0;
	float half_unit_size = 0.5f;
	for (int i = -grid_size; i <= grid_size; i += 2) {
		float size_f = half_unit_size  * grid_size;
		float i_f = half_unit_size * i;

		Vec3f pt = { -size_f, 0, i_f };
		Vec3f pt2 = { size_f, 0, i_f };

		opengl->grid_mesh.pos[index + 0] = pt;
		opengl->grid_mesh.pos[index + 1] = pt2;


		pt = Vec3f(i_f, 0, -size_f);
		pt2 = Vec3f(i_f, 0, size_f);

		opengl->grid_mesh.pos[index + 2] = pt;
		opengl->grid_mesh.pos[index + 3] = pt2;

		opengl->grid_mesh.color[index + 0] = color_white;
		opengl->grid_mesh.color[index + 1] = color_white;
		opengl->grid_mesh.color[index + 2] = color_white;
		opengl->grid_mesh.color[index + 3] = color_white;



		index += 4;

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
	
	return true;
}

bool destroy_opengl_renderer(OpenGLRenderer* opengl) {
	SDL_GL_DeleteContext(opengl->gl_context);

	
	
	
	

	
	for (int i = 0; i < opengl->texture_count; i++) {
		glDeleteTextures(1, &opengl->textures[i]);
	}
	opengl->texture_count = 0;


	for (int i = 0; i < opengl->shader_count; i++) {
		delete_gl_program(&opengl->shaders[i]);
	}
	opengl->shader_count = 0;

	for (int i = 0; i < opengl->vbo_count; i++) {
		glDeleteBuffers(1, &opengl->vbos[i]);
	}
	opengl->vbo_count = 0;

	for (int i = 0; i < opengl->ebo_count; i++) {
		glDeleteBuffers(1, &opengl->ebos[i]);
	}
	opengl->ebo_count = 0;

	for (int i = 0; i < opengl->vao_count; i++) {
		glDeleteVertexArrays(1, &opengl->vaos[i]);
	}
	opengl->vao_count = 0;
	for (int i = 0; i < opengl->fbo_count; i++) {
		glDeleteFramebuffers(1, &opengl->fbos[i]);
	}
	opengl->fbo_count = 0;
	for (int i = 0; i < opengl->rbo_count; i++) {
		glDeleteRenderbuffers(1, &opengl->rbos[i]);
	}
	opengl->rbo_count = 0;

	arena_free(&opengl->arena);

	
 
	return true;
}

void opengl_debug_render(OpenGLRenderer* opengl, Vec2i viewport_size) {

	glViewport(0, 0, viewport_size.x, viewport_size.y);


	Camera camera = *opengl->render_world->camera;
	Mat4x4f model_mat;
	Mat4x4f view_mat = camera.view_mat;
	Mat4x4f projection_mat = perspective(camera.near, camera.far, camera.fov, camera.aspect_ratio);
	
	Mat4x4f mvp_mat = projection_mat * view_mat * model_mat;
	
	
	GLShader debug_shader = opengl->shaders[opengl->render_world->debug_shader_res.handle];
	glBindVertexArray(opengl->vaos[opengl->render_world->debug_grid_vao_res.handle]);
	glUseProgram(debug_shader.program);

	glUniformMatrix4fv(glGetUniformLocation(debug_shader.program, "transform"), 1, GL_FALSE, mvp_mat.mat1d);



	// Draw grid
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->debug_grid_vbo_res.handle]);


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

		glLineWidth(4);
		glDrawArrays(GL_LINES, opengl->axes_pos_offset, opengl->grid_mesh.vertex_count - opengl->axes_pos_offset);




		// Draw light


		Vec3f dir_light_line[2];
		Vec3f dir_light_line_color[2];

		dir_light_line[1] = opengl->render_world->test_light.dir_light.direction;
		dir_light_line[0] = Vec3f(0, 0, 0);


		dir_light_line_color[0] = Vec3f(1, 1, 1);
		dir_light_line_color[1] = Vec3f(1, 1, 1);
		glBufferData(GL_ARRAY_BUFFER,
			4 * sizeof(Vec3f),
			NULL,
			GL_STATIC_DRAW);

		// Buffer pos data
		glBufferSubData(GL_ARRAY_BUFFER,
			0,
			2 * sizeof(Vec3f),
			&dir_light_line);


		// Buffer color data
		glBufferSubData(GL_ARRAY_BUFFER,
			2 * sizeof(Vec3f),
			2 * sizeof(Vec3f),
			&dir_light_line_color);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(2 * sizeof(Vec3f)));
		glEnableVertexAttribArray(1);



		glDrawArrays(GL_LINES, 0, 2);




		for (int i = 0; i < opengl->render_world->render_mesh_count; i++) {
			RenderMesh render_mesh = opengl->render_world->render_mesh_list[i];
			Mat4x4f* world_mat = render_mesh.world;

			Vec3f pos;

			pos.x = world_mat->mat2d[3][0];
			pos.y = world_mat->mat2d[3][1];
			pos.z = world_mat->mat2d[3][2];


			Vec3f right;
			right.x = world_mat->mat2d[0][0];
			right.y = world_mat->mat2d[0][1];
			right.z = world_mat->mat2d[0][2];

			Vec3f up;
			up.x = world_mat->mat2d[1][0];
			up.y = world_mat->mat2d[1][1];
			up.z = world_mat->mat2d[1][2];

			Vec3f forward;
			forward.x = world_mat->mat2d[2][0];
			forward.y = world_mat->mat2d[2][1];
			forward.z = world_mat->mat2d[2][2];



			gizmo_render_axis(opengl, pos, forward, up, right);

		}
	}




}

static void opengl_render_scene(OpenGLRenderer* opengl, Vec2i viewport_size, bool light_pass) {
	Camera* camera = opengl->render_world->camera;
	Vec3f cam_pos = opengl->render_world->cam_pos;

	DirectionalLight test_light = opengl->render_world->test_light.dir_light;



	int draw_type = GL_TRIANGLES;
	if (opengl->draw_lines) {
		draw_type = GL_LINES;
	} else {
		draw_type = GL_TRIANGLES;
	}

	

	Mat4x4f view_mat = camera->view_mat;
	Mat4x4f projection_mat = perspective(camera->near, camera->far, camera->fov, camera->aspect_ratio);
	Mat4x4f pv_mat;
	
	
	if (light_pass) {
		// While a directional light has no position, we treat the direction like a postion, where the direction of the light is the vector to the origin
		view_mat = look_at(Vec3f(0, 0, 0), test_light.direction, Vec3f_Up);
		projection_mat = ortho(-camera->far, camera->far, viewport_size.y * 0.01f, -viewport_size.y* 0.01f, viewport_size.x* 0.01f, -viewport_size.x* 0.01f);
		pv_mat = projection_mat * view_mat;
		opengl->render_world->light_space_mat = pv_mat;
	} else {
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->shadow_map_res.handle]);
		pv_mat = projection_mat * view_mat;
	}

	



	GLint current_shader = light_pass ?
		opengl->shaders[opengl->render_world->shadow_shader_res.handle].program
		: opengl->shaders[opengl->render_world->texture_shader_res.handle].program;


	
	// Texture Shader
	glBindVertexArray(opengl->vaos[opengl->render_world->VAO.handle]);

	glUseProgram(current_shader);
		
	glUniformMatrix4fv(glGetUniformLocation(current_shader, "projection_view"), 1, GL_FALSE, pv_mat.mat1d);
	
	glUniform3f(glGetUniformLocation(current_shader, "camera_pos"), uniform3f_pack(cam_pos));
	glUniform3f(glGetUniformLocation(current_shader, "light_pos"), uniform3f_pack(test_light.direction));
	if (!light_pass) {
		glUniformMatrix4fv(glGetUniformLocation(current_shader, "light_space_mat"), 1, GL_FALSE, opengl->render_world->light_space_mat.mat1d);

		

		GLuint uniform_index = glGetUniformLocation(current_shader, "shadowMap");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->shadow_map_res.handle]);
		glUniform1i(uniform_index, 0);

		
		uniform_index = glGetUniformLocation(current_shader, "albedo_map");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->albedo_map_res.handle]);
		glUniform1i(uniform_index, 1);

		uniform_index = glGetUniformLocation(current_shader, "normal_map");
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->normal_map_res.handle]);
		glUniform1i(uniform_index, 2);


		uniform_index = glGetUniformLocation(current_shader, "metallic_map");
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->metallic_map_res.handle]);
		glUniform1i(uniform_index, 3);

		uniform_index = glGetUniformLocation(current_shader, "roughness_map");
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->roughness_map_res.handle]);
		glUniform1i(uniform_index, 4);

		uniform_index = glGetUniformLocation(current_shader, "ao_map");
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->ao_map_res.handle]);
		glUniform1i(uniform_index, 5);

		uniform_index = glGetUniformLocation(current_shader, "irradiance_map");
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->textures[opengl->render_world->irradiance_map_res.handle]);
		glUniform1i(uniform_index, 6);

		uniform_index = glGetUniformLocation(current_shader, "prefilter_map");
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->textures[opengl->render_world->prefiler_map_res.handle]);
		glUniform1i(uniform_index, 7);

		uniform_index = glGetUniformLocation(current_shader, "brdf_lut");
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, opengl->textures[opengl->render_world->brdf_lut_res.handle]);
		glUniform1i(uniform_index, 8);


	}

		

	for (int i = 0; i < opengl->render_world->render_mesh_count; i++) {
		RenderMesh render_mesh = opengl->render_world->render_mesh_list[i];
		
		StaticMesh* mesh = render_mesh.mesh;
		Mat4x4f* world_mat = render_mesh.world;
		
		if (mesh->vertex_count == 0) { continue; }
		glUniformMatrix4fv(glGetUniformLocation(current_shader, "model"), 1, GL_FALSE, world_mat->mat1d);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->vertex_count * sizeof(Vec3f)
			+ mesh->vertex_count * sizeof(Vec2f)
			+ mesh->vertex_count * sizeof(Vec3f),
			NULL,
			GL_DYNAMIC_DRAW);
		
		glBufferSubData(GL_ARRAY_BUFFER,
			0,
			mesh->vertex_count * sizeof(Vec3f),
			mesh->pos);
		
		//crashes because vertex count is bigger than size of normal / texcoords
		
		glBufferSubData(GL_ARRAY_BUFFER,
			mesh->vertex_count * sizeof(Vec3f),
			mesh->vertex_count * sizeof(Vec2f),
			mesh->texcoords);
		
		glBufferSubData(GL_ARRAY_BUFFER,
			mesh->vertex_count * sizeof(Vec3f) + mesh->vertex_count * sizeof(Vec2f),
			mesh->vertex_count * sizeof(Vec3f),
			mesh->normal);
		
		
		
		
		
		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
		glEnableVertexAttribArray(0);
		
		// Texcoords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), cast(GLvoid*)(mesh->vertex_count * sizeof(Vec3f)));
		glEnableVertexAttribArray(1);
		
		// Normals
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)(mesh->vertex_count * sizeof(Vec3f) + mesh->vertex_count * sizeof(Vec2f)));
		glEnableVertexAttribArray(2);
		
		
		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->ebos[opengl->render_world->EBO.handle]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->index_count * sizeof(Vec3i),
			mesh->indices,
			GL_DYNAMIC_DRAW);
		
		
		
		glDrawElements(draw_type, 3 * mesh->index_count, GL_UNSIGNED_INT, 0);
	}


	// Skip skybox drawing if a light pass
	if (light_pass) return;
	// Draw skybox

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	
	GLShader skybox_shader = opengl->shaders[opengl->render_world->skybox_shader_res.handle];
	glBindVertexArray(opengl->vaos[opengl->render_world->skybox_vao_res.handle]);
	glUseProgram(skybox_shader.program);
	glUniformMatrix4fv(glGetUniformLocation(skybox_shader.program, "projection"), 1, GL_FALSE, projection_mat.mat1d);
	glUniformMatrix4fv(glGetUniformLocation(skybox_shader.program, "view"), 1, GL_FALSE, view_mat.mat1d);
	

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, opengl->textures[opengl->render_world->env_cubemap_res.handle]);
	glUniform1i(glGetUniformLocation(skybox_shader.program, "skybox"), 0);
	
	
	
	glBindBuffer(GL_ARRAY_BUFFER, opengl->vbos[opengl->render_world->skybox_vbo_res.handle]);
	glBufferData(GL_ARRAY_BUFFER,
		opengl->render_world->skymap->cube.vertex_count * sizeof(Vec3f),
		opengl->render_world->skymap->cube.pos,
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl->ebos[opengl->render_world->skybox_ebo_res.handle]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		opengl->render_world->skymap->cube.index_count * sizeof(Vec3i),
		opengl->render_world->skymap->cube.indices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), cast(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glFrontFace(GL_CW);

	glDrawElements(GL_TRIANGLES, 3 * opengl->render_world->skymap->cube.index_count, GL_UNSIGNED_INT, 0);


	glFrontFace(GL_CCW);

	glDepthFunc(GL_LESS); // set depth function back to default


	

}

void init_gl_resource_arrays(OpenGLRenderer* opengl) {
	
	opengl->obj_capacity = 20;

	opengl->texture_count = 0;
	opengl->shader_count = 0;
	opengl->vbo_count = 0;
	opengl->ebo_count = 0;
	opengl->vao_count = 0;
	opengl->fbo_count = 0;
	opengl->rbo_count = 0;

	opengl->textures = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);
	opengl->shaders = cast(GLShader*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLShader), 4);
	opengl->vbos = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);
	opengl->ebos = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);
	opengl->vaos = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);
	opengl->fbos = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);
	opengl->rbos = cast(GLuint*) stack_alloc(&opengl->stack_allocator, opengl->obj_capacity * sizeof(GLuint), 4);


}

void opengl_render(OpenGLRenderer* opengl, Vec2i viewport_size, bool render_debug) {
	
	
	// Shadow pass


	//glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl->fbos[opengl->render_world->shadow_fbo_res.handle]);
	glViewport(0, 0, SHADOW_WIDTH_RES, SHADOW_HEIGHT_RES);
	glClear(GL_DEPTH_BUFFER_BIT);
	opengl_render_scene(opengl, viewport_size, true);
	//glCullFace(GL_BACK); // don't forget to reset original culling face
	

	// Normal lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, viewport_size.x, viewport_size.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	opengl_render_scene(opengl, viewport_size, false);
	
	if (render_debug) {
		opengl_debug_render(opengl, viewport_size);
	}

	clear_render_world(opengl);
	
	
}

void clear_render_world(OpenGLRenderer* opengl) {
	opengl->render_world->render_mesh_count = 0;
}

void opengl_swap_buffer(OpenGLRenderer* opengl) {
	SDL_GL_SwapWindow(opengl->sdl_window);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
