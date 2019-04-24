#pragma once

#include "Core/Renderer/Renderer.h"
#include "Common/stretchy_buffer.h"
#include "Logger.h"


void init_backend_renderer(Renderer* renderer, SDL_Window* sdl_window) {
	switch (renderer->type) {
		case BackenedRendererType::Software: {
			//MemoryEnginePartition parition_start = give_memory_partition(engine, RENDERER_MEMORY);
			//return init_software_renderer(engine->window.sdl_window, &engine->renderer.software_renderer, engine->window.size, parition_start.start_ptr, parition_start.partition_size);
			break;
		}
		case BackenedRendererType::OpenGL: {
			map_init(&renderer->render_world.material_res_map);
			map_grow(&renderer->render_world.material_res_map, 8);

			renderer->render_world.resources = NULL;
			renderer->render_world.resources_count = 0;

			renderer->render_world.material_res = NULL;
			renderer->render_world.material_res_count = 0;

			init_opengl_renderer(sdl_window, &renderer->opengl, &renderer->render_world);
			break;
		}
		default:
			break;
	}

}

void destory_backend_renderer(Renderer* renderer) {
	LOG_INFO("RENDERER", "Destroying renderer");
	switch (renderer->type) {
		case BackenedRendererType::Software: {
			
			break;
		}
		case BackenedRendererType::OpenGL: {
			map_destroy(&renderer->render_world.material_res_map);
			destroy_opengl_renderer(&renderer->opengl);
			break;
		}
		default:
			break;
	}
}

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
		case BackenedRendererType::OpenGL:
			handle = gl_create_fbo(&renderer->opengl);
			break;
	}
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::FRAME_BUFFER;
	return handle;
}

RenderResource create_render_target(Renderer* renderer, u32 width, u32 height) {
	RenderResource handle;

	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			handle = gl_create_rbo(&renderer->opengl, width, height);
			break;
	}

	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::RENDER_TARGET;
	return handle;
}

RenderResource create_vertex_decl(Renderer* renderer) {

	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			handle = gl_create_vao(&renderer->opengl);
			break;
	}
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::VERTEX_DECLARATION;
	return handle;
}

RenderResource create_vertex_buffer(Renderer* renderer) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			handle = gl_create_vbo(&renderer->opengl);
			break;
	}
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::VERTEX_BUFFER;
	return handle;
}

RenderResource create_index_buffer(Renderer* renderer) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			break;
	}
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::INDEX_BUFFER;
	return handle;
}


bool is_material_loaded(Renderer* renderer, MaterialID id) {
	MapResult<RenderMaterialResource*> result = map_get(&renderer->render_world.material_res_map, id.id);
	return result.found;
}

RenderMaterialResource create_material_resource(Renderer* renderer, InternalMaterial* material) {
	
	TextureID no_texture;
	
	RenderMaterialResource material_handle;
	
	if (material->tx_albedo != NULL) {
		material_handle.albedo = create_texture_resource(renderer, material->tx_albedo, false);
	}
	if (material->tx_normal != NULL) {
		material_handle.normal = create_texture_resource(renderer, material->tx_normal, false);
	}
	if (material->tx_metal != NULL) {
		material_handle.metallic = create_texture_resource(renderer, material->tx_metal, false);
	}
	if (material->tx_roughness != NULL) {
		material_handle.roughness = create_texture_resource(renderer, material->tx_roughness, false);
	}
	if (material->tx_ao != NULL) {
		material_handle.ao = create_texture_resource(renderer, material->tx_ao, false);
	}
	
	sb_push(renderer->render_world.material_res, material_handle);
	
	RenderMaterialResource* handle = &sb_last(renderer->render_world.material_res);
	map_put(&renderer->render_world.material_res_map, material->material.id.id, handle);
	renderer->render_world.material_res_count++;

	return material_handle;
}

RenderResource create_texture_resource(Renderer* renderer, Texture2D* texture, bool mipmap) {
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			handle = gl_create_texture(&renderer->opengl, texture, mipmap);
			break;
	}
	
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::TEXTURE;
	return handle;
}

RenderResource create_shader_resource(Renderer* renderer, const char* vertex_file, const char* fragment_file, const char* geometry_file) {
	
	RenderResource handle;
	
	switch (renderer->type) {
		case BackenedRendererType::OpenGL:
			handle = gl_create_shader(&renderer->opengl, vertex_file, fragment_file, geometry_file);
			break;
	}
	renderer->render_world.resources_count++;
	handle.type = RenderResourceType::SHADER;
	return handle;
}

