#pragma once


#include "../../Math/Vec.h"

typedef struct Camera {
	// TODO: should move all vec3s to vec4s for better alignment
	Vec4f pos;
	Vec4f dir;
	Vec4f rotation;

	float near;
	float far;
	float fov;
	float aspect_ratio;

} Camera;


// TODO: move this to it;s own c file
void init_camera(Camera* camera, Vec4f pos, Vec4f dir, Vec4f orientation, float near, float far, float fov, float aspect_ratio);
void init_camera_default(Camera* camera);
void set_camera_pos(Camera* camera, Vec4f pos);



void move_camera(Camera* camera, Vec3f direction, float delta_time);