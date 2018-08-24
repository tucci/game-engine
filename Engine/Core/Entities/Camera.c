#pragma once

#include "Camera.h"
#include "../../Common/common_macros.h"



void init_camera(Camera* camera,
	Vec3f pos,
	Vec3f dir,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	camera->pos = pos;
	camera->forward = dir;

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}


void init_camera_default(Camera* camera) {
	
	camera->pos = make_vec3f( 0, 0, 0);
	camera->forward = make_vec3f( 0, 0, -1);

	camera->up = make_vec3f(0, 1, 0);
	camera->right = v3_cross(camera->forward, camera->up);
	
	

	camera->near = 0.1f;
	camera->far = 100.0f;

	camera->fov = 60.0f;
	camera->aspect_ratio = 1.0f;
}

void set_camera_pos(Camera* camera, Vec3f pos) {
	camera->pos = pos;
}



void move_camera_in_direction(Camera* camera, Vec3f direction, float delta_time) {
	camera->pos = v3_add(camera->pos, v3_multiply(1 * delta_time, direction));
}