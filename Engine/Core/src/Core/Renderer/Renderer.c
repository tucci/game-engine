#pragma once

#include "Core/Renderer/Renderer.h"


void push_render_object(Renderer* renderer, RenderMesh desc) {
	renderer->render_world.render_mesh_list[renderer->render_world.render_mesh_count] = desc;
	renderer->render_world.render_mesh_count++;
}

void create_skymap(Renderer* renderer, HDR_SkyMap* skymap) {
	renderer->render_world.skymap = skymap;
	gl_init_hdr_map(&renderer->opengl, skymap);
}

void push_camera(Renderer* renderer, Camera* camera, Vec3f pos) {
	
	renderer->render_world.camera = camera;
	renderer->render_world.cam_pos = pos;
}

void push_light(Renderer* renderer, Light light) {
	renderer->render_world.test_light = light;
}

void create_shadowmap(Renderer* renderer) {
	gl_init_shadow_maps(&renderer->opengl);
}

RenderResource create_frame_buffer(Renderer* renderer) {
	RenderResource handle;

	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_fbo(&renderer->opengl);
			break;
	}

	handle.type = RenderResourceType_FRAME_BUFFER;
	return handle;
}

RenderResource create_render_target(Renderer* renderer, uint32_t width, uint32_t height) {
	RenderResource handle;

	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_rbo(&renderer->opengl, width, height);
			break;
	}

	handle.type = RenderResourceType_RENDER_TARGET;
	return handle;
}

RenderResource create_vertex_decl(Renderer* renderer) {

	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_vao(&renderer->opengl);
			break;
	}

	handle.type = RenderResourceType_VERTEX_DECLARATION;
	return handle;
}

RenderResource create_vertex_buffer(Renderer* renderer) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_vbo(&renderer->opengl);
			break;
	}
	handle.type = RenderResourceType_VERTEX_BUFFER;
	return handle;
}

RenderResource create_index_buffer(Renderer* renderer) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			break;
	}
	handle.type = RenderResourceType_INDEX_BUFFER;
	return handle;
}

RenderResource create_texture(Renderer* renderer, SimpleTexture* texture, bool mipmap) {
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_texture(&renderer->opengl, texture, mipmap);
			break;
	}
	
	handle.type = RenderResourceType_TEXTURE;
	return handle;
}

RenderResource create_shader(Renderer* renderer, const char* vertex_file, const char* fragment_file) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRenderer_OpenGL:
			handle = gl_create_shader(&renderer->opengl, vertex_file, fragment_file);
			break;
	}
	handle.type = RenderResourceType_SHADER;
	return handle;
}

