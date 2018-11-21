#pragma once

#include "Asset/Asset.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Lights.h"
#include "Core/ECS/Component/Skybox.h"

#include "Math/Mat.h"
#include "Math/Vec.h"	

typedef struct RenderMesh {
	int material_id;
	StaticMesh* mesh;
	Mat4x4f* world;
} RenderMesh;

// Concept take and slightly modifief from the stingray engine blog
//https://bitsquid.blogspot.com/2017/02/stingray-renderer-walkthrough-2.html
typedef enum RenderResourceType {
	RenderResourceType_TEXTURE,
	RenderResourceType_SHADER,
	RenderResourceType_VERTEX_BUFFER,
	RenderResourceType_INDEX_BUFFER,
	RenderResourceType_VERTEX_DECLARATION,
	RenderResourceType_RENDER_TARGET,
	RenderResourceType_FRAME_BUFFER,

	RenderResourceType_DEPENDENT_RENDER_TARGET,
	RenderResourceType_BACK_BUFFER_WRAPPER,
	RenderResourceType_CONSTANT_BUFFER,
	RenderResourceType_RAW_BUFFER,
	RenderResourceType_BATCH_INFO,
	RenderResourceType_NOT_INITIALIZED = 0xFFFFFFFF
} RenderResourceType;

typedef struct RenderResource {
	// TODO: eventually move the type into the higher 8 bits of the handle,
	// and use lower 24bits of the handle as an index to an array
	RenderResourceType type;
	u32 handle;
} RenderResource;

// the world of objects we need to render and states
typedef struct RenderWorld {
	// TODO: we probably want to hold the meshes by materials first for easier sorting?
	int render_mesh_count;
	int render_mesh_capacity;
	RenderMesh* render_mesh_list;

	Camera* camera;
	Vec3f cam_pos;
	Light test_light;
	HDR_SkyMap* skymap;

	// Shadow map
	Mat4x4f light_space_mat;
	RenderResource shadow_shader_res;
	RenderResource shadow_fbo_res;
	RenderResource shadow_map_res;

	// IBL cubemaps
	RenderResource env_cubemap_res;
	RenderResource hdr_skymap_res;
	RenderResource irradiance_map_res;
	RenderResource prefiler_map_res;
	RenderResource brdf_lut_res;

	

	// IBL shaders
	RenderResource equirectangular_shader_res;
	RenderResource skybox_shader_res;
	RenderResource irradiance_shader_res;
	RenderResource prefilter_shader_res;
	RenderResource brdf_shader_res;

	// IBL Buffers
	RenderResource capture_fbo_res;
	RenderResource capture_rbo_res;
	
	RenderResource skybox_vao_res;
	RenderResource skybox_vbo_res;
	RenderResource skybox_ebo_res;


	RenderResource albedo_map_res;
	RenderResource normal_map_res;
	RenderResource metallic_map_res;
	RenderResource roughness_map_res;
	RenderResource ao_map_res;

	RenderResource texture_shader_res;

	RenderResource VAO;
	RenderResource VBO; // id to the Buffer that stores our vertices
	RenderResource EBO; // id to the Buffer that stores our indices


	RenderResource debug_shader_res;
	RenderResource debug_grid_vao_res;
	RenderResource debug_grid_vbo_res;

} RenderWorld;