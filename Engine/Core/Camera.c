#pragma once

#include "Camera.h"


void init_camera(Camera* camera,
	Vec3f pos,
	Vec3f dir,
	Vec3f orientation,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	camera->pos = (Vec4f) { pos.x, pos.y, pos.z, 1 };
	camera->dir = dir;
	camera->rotation = orientation;

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}


void init_camera_default(Camera* camera) {
	camera->pos = (Vec4f) { 0, 0, 0, 1 };
	camera->dir = (Vec3f) { 0, 0, -1 };
	camera->rotation = Vec3f_Zero;

	camera->near = 0.1f;
	camera->far = 100.0f;

	camera->fov = 60.0f;
	camera->aspect_ratio = 1.0f;
}

void set_camera_pos(Camera* camera, Vec3f pos) {
	camera->pos = (Vec4f) { pos.x, pos.y, pos.z, 1 };
}