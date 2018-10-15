#pragma once

#include "Core/Renderer/RenderInterface.h"

#include "Core/Renderer/software_renderer/SoftwareRenderer.h"
#include "Core/Renderer/gl/OpenGLRenderer.h"




typedef enum BackenedRendererType {
	BackenedRenderer_Software,
	BackenedRenderer_OpenGL
} BackenedRendererType;



typedef struct Renderer {
	BackenedRendererType type;
	union {
		SoftwareRenderer software_renderer;
		OpenGLRenderer opengl;
		
	};	
	
	RenderWorld render_world;
	Renderer() {};

} Renderer;



void push_render_object(Renderer* renderer, RenderMesh desc);
// TODO: these are just tests, anc should be removed once we have a better api
void create_shadowmap(Renderer* renderer);
void create_skymap(Renderer* renderer, HDR_SkyMap* skymap);
void push_camera(Renderer* renderer, Camera* camera, Vec3f pos);
void push_light(Renderer* renderer, Light light);

// NOTE: these create the resources on the gpu
RenderResource create_frame_buffer(Renderer* renderer);
RenderResource create_render_target(Renderer* renderer, uint32_t width, uint32_t height);
RenderResource create_vertex_decl(Renderer* renderer);
RenderResource create_vertex_buffer(Renderer* renderer);
RenderResource create_index_buffer(Renderer* renderer);
// When we create a texture, we copy the texture to the gpu memory
RenderResource create_texture(Renderer* renderer, SimpleTexture* texture, bool mipmap);
// TODO: implement generic shader abstraction filenames
RenderResource create_shader(Renderer* renderer, const char* vertex_file, const char* fragment_file);

 
