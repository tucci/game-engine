#pragma once

#include "Core/ECS/EntityManager.h"


Camera* get_camera(EntityManager* manager, Entity entity);
void set_camera_params(EntityManager* manager, Entity entity,
	float near_clip,
	float far_clip,
	float fov,
	float aspect_ratio);

void set_camera_projection(EntityManager* manager, Entity entity, CameraProjection projection);

void job_compute_camera_view_matrices(EntityManager* manager);