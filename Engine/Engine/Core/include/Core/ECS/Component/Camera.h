#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"



#include "Common/Map.h"

#include "Core/ECS/Entity.h"

struct EntityManager;
struct RenderResource;

struct CameraID {
	u64 id;
};

enum class CameraProjection {
	Perspective,
	Orthographic
};

struct Camera {
	
	//Transform transform;
	Entity entity_ref;
	float near_clip;
	float far_clip;
	float fov;
	float aspect_ratio;

	CameraProjection projection;
	float size;
	//float top;
	//float bottom;
	//float left;
	//float right;

	// Camera doens't own the framebuffer pointer
	RenderResource* framebuffer;
	Mat4x4f view_mat;
	Camera() {}
	Camera(Entity ref) {
		projection = CameraProjection::Perspective;
		entity_ref = ref;
		near_clip = 0.0001f;
		far_clip = 100.0f;
		fov = 90.0f;
		aspect_ratio = 1.0f;
		view_mat = Mat4x4f();
		size = 1.0f;
		
	}
};


void init_camera_params(Camera* camera, float near_clip, float far_clip, float fov, float aspect_ratio);


struct CameraManager {
	CompactMap<u64> id_map;
	Entity* entitys;
	Camera* enabled_cameras;
	u64 total_count;
	u64 enabled_count;

};



void init_camera_manager(CameraManager* manager);
void destroy_camera_manager(CameraManager* manager);
bool entity_add_camera_component(CameraManager* manager, Entity entity);
bool entity_remove_camera_component(CameraManager* manager, Entity entity);
void enable_camera_component(CameraManager* manager, Entity entity, bool enabled);

