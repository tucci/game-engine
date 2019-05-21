#pragma once

#include "Core/ECS/EntityManager.h"


Camera* get_camera(EntityManager* manager, Entity entity);


void set_camera(EntityManager* manager, Entity entity, Camera camera);

void set_camera_params(EntityManager* manager, Entity entity,
	float near_clip,
	float far_clip,
	float fov,
	float aspect_ratio);


float get_camera_ortho_size(EntityManager* manager, Entity entity);
void set_camera_ortho_size(EntityManager* manager, Entity entity, float size);
//void set_camera_ortho_params(EntityManager* manager, Entity entity, float top, float bottom, float left, float right);
void set_camera_projection(EntityManager* manager, Entity entity, CameraProjection projection);
void set_camera_aspect_ratio(EntityManager* manager, Entity entity, float aspect_ratio);
void set_camera_framebuffer(EntityManager* manager, Entity entity, RenderResource* framebuffer);

void job_compute_camera_view_matrices(EntityManager* manager);