#pragma once


#include "Common/stretchy_buffer.h"
#include "Core/Renderer/RenderInterface.h"
#include "Core/Renderer/software_renderer/SoftwareRenderer.h"
#include "Core/Renderer/gl/OpenGLRenderer.h"




enum class BackenedRendererType {
	Software,
	OpenGL
};



struct Renderer {
	BackenedRendererType type;
	union {
		SoftwareRenderer software_renderer;
		OpenGLRenderer opengl;
	};	

	
	RenderWorld render_world;
	Renderer() {};

};



void init_backend_renderer(Renderer* renderer, SDL_Window* sdl_window);
void destory_backend_renderer(Renderer* renderer);


void push_render_object(Renderer* renderer, RenderMesh desc);
// TODO: these are just tests, anc should be removed once we have a better api
void create_shadowmap(Renderer* renderer);
void create_skymap(Renderer* renderer, HDR_SkyMap* skymap);
void push_camera(Renderer* renderer, Camera* camera, Vec3f pos);
void push_light(Renderer* renderer, Light light);

// NOTE: these create the resources on the gpu
RenderResource create_frame_buffer(Renderer* renderer, u32 width, u32 height);
RenderResource create_frame_buffer(Renderer* renderer, u32 attachment_count, FrameBufferAttachement* attacments);
//RenderResource create_render_buffer(Renderer* renderer, u32 width, u32 height);
RenderResource create_vertex_decl(Renderer* renderer);
RenderResource create_vertex_buffer(Renderer* renderer);
RenderResource create_index_buffer(Renderer* renderer);



bool is_material_loaded(Renderer* renderer, MaterialID id);

RenderMaterialResource create_material_resource(Renderer* renderer, InternalMaterial* material);

// When we create a texture, we copy the texture to the gpu memory
// however it does not free the texture memory on the cpu side. the caller must manually free it
RenderResource create_texture_resource(Renderer* renderer, Texture2D* texture, bool mipmap);
// TODO: implement generic shader abstraction filenames
RenderResource create_shader_resource(Renderer* renderer, const char* vertex_file, const char* fragment_file);


void* render_resource_to_id(Renderer* renderer, RenderResource resource);

