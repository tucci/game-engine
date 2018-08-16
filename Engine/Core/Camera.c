#pragma once

#include "Camera.h"
#include "../Common/common_macros.h"


void init_camera(Camera* camera,
	Vec4f pos,
	Vec4f dir,
	Vec4f orientation,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	camera->pos = pos;
	camera->dir = dir;
	camera->rotation = orientation;

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}


void init_camera_default(Camera* camera) {
	camera->pos = ToVec4f( 0, 0, 0, 1);
	camera->dir = ToVec4f( 0, 0, -1, 0);
	camera->rotation = Vec4f_Zero;

	camera->near = 0.1f;
	camera->far = 100.0f;

	camera->fov = 60.0f;
	camera->aspect_ratio = 1.0f;
}

void set_camera_pos(Camera* camera, Vec4f pos) {
	camera->pos = pos;
}