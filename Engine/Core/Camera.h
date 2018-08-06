#pragma once

typedef struct Camera {
	Vec4f pos;
	Vec3f dir;
	Vec3f rotation;

	float near;
	float far;
	float fov;
	float aspect_ratio;
} Camera;


// TODO: move this to it;s own c file
void inline init_camera(Camera* camera,
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


void inline init_camera_default(Camera* camera) {
	camera->pos = (Vec4f) { 0, 0, 0, 1 };
	camera->dir = (Vec3f) { 0, 0, -1 };
	camera->rotation = Vec3f_Zero;
		  
	camera->near = 0.1f;
	camera->far = 100.0f;
		  
	camera->fov = 90.0f;
	camera->aspect_ratio = 1.0f;
}

void inline set_camera_pos(Camera* camera, Vec3f pos) {
	camera->pos = (Vec4f) { pos.x, pos.y, pos.z, 1 };
}