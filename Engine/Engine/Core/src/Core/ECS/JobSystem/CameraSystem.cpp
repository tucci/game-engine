#pragma once

#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/TransformSystem.h"


Camera* get_camera(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);
	if (!result.found) return NULL;
	u64 index = result.value;
	return &manager->camera_manager.enabled_cameras[index];
}

void set_camera(EntityManager* manager, Entity entity, Camera camera) {
	set_camera_params(manager, entity, camera.near_clip, camera.far_clip, camera.fov, camera.aspect_ratio);
	set_camera_projection(manager, entity, camera.projection);
	set_camera_ortho_size(manager, entity, camera.size);
}


void set_camera_params(EntityManager* manager, Entity entity,
	float near_clip,
	float far_clip,
	float fov,
	float aspect_ratio) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);
	
	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];

	cam->near_clip = near_clip;
	cam->far_clip = far_clip;
	cam->fov = fov;
	cam->aspect_ratio = aspect_ratio;
}

float get_camera_ortho_size(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	return cam->size;
}

void set_camera_ortho_size(EntityManager* manager, Entity entity, float size) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	cam->size = size;
}

//void set_camera_ortho_params(EntityManager* manager, Entity entity, float top, float bottom, float left, float right) {
//	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);
//
//	u64 index = result.value;
//	Camera* cam = &manager->camera_manager.enabled_cameras[index];
//
//	cam->top = top;
//	cam->bottom = bottom;
//	cam->left = left;
//	cam->right = right;
//}

void set_camera_projection(EntityManager* manager, Entity entity, CameraProjection projection) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	cam->projection = projection;
}

void set_camera_aspect_ratio(EntityManager* manager, Entity entity, float aspect_ratio) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	cam->aspect_ratio = aspect_ratio;
}

void set_camera_framebuffer(EntityManager* manager, Entity entity, RenderResource* framebuffer) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	cam->framebuffer = framebuffer;
}

Mat4x4f* get_camera_view_matrix(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];
	return &cam->view_mat;
}

Mat4x4f get_camera_projection_matrix(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);

	u64 index = result.value;
	Camera* cam = &manager->camera_manager.enabled_cameras[index];

	return perspective(cam->near_clip, cam->far_clip, cam->fov, cam->aspect_ratio);
}

void job_compute_camera_view_matrices(EntityManager* manager) {
	for (int i = 0; i < manager->camera_manager.enabled_count; i++) {
		Camera* cam = &manager->camera_manager.enabled_cameras[i];
		Entity e = manager->camera_manager.enabled_cameras[i].entity_ref;
		Vec3f cam_pos = get_position(manager, e);
		// Since camera is looking now -z, we want to move in that direction instead
		// so we need to flip the forward
		Vec3f cam_forward = -forward(manager, e);
		Vec3f cam_up = up(manager, e);

		
		cam->view_mat = look_at(cam_pos, cam_pos + cam_forward, cam_up);
	}
}
