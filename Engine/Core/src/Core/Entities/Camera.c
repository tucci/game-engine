#pragma once

#include "Core/Entities/Camera.h"

#include "Common/common_macros.h"



void init_camera(Camera* camera,
	Vec3f pos,
	Vec3f dir,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	
	camera->transform.position = pos;
	camera->transform.forward = dir;

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}


void init_camera_default(Camera* camera) {
	
	camera->transform.position = Vec3f( 0, 0, 0);

	camera->transform.forward = Vec3f( 0, 0, -1);
	camera->transform.up = Vec3f(0, 1, 0);
	camera->transform.right = cross(camera->transform.forward, camera->transform.up);
	
	

	camera->near = 0.1f;
	camera->far = 100.0f;

	camera->fov = 60.0f;
	camera->aspect_ratio = 1.0f;
}

void set_camera_pos(Camera* camera, Vec3f pos) {
	camera->transform.position = pos;
}



void move_camera_in_direction(Camera* camera, Vec3f direction, float delta_time) {
	camera->transform.position = camera->transform.position + (10 * delta_time) * direction;
}