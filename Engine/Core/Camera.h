#pragma once


#include "../Math/Vec.h"

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
void init_camera(Camera* camera, Vec3f pos, Vec3f dir, Vec3f orientation, float near, float far, float fov, float aspect_ratio);
void init_camera_default(Camera* camera);
void set_camera_pos(Camera* camera, Vec3f pos);