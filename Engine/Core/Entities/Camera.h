#pragma once


#include "../../Math/Vec.h"

typedef struct Camera {
	Vec4f pos;
	Vec4f dir;
	Vec4f rotation;

	float near;
	float far;
	float fov;
	float aspect_ratio;

} Camera;


void init_camera(Camera* camera, Vec4f pos, Vec4f dir, Vec4f orientation, float near, float far, float fov, float aspect_ratio);
void init_camera_default(Camera* camera);
void set_camera_pos(Camera* camera, Vec4f pos);

void move_camera_in_direction(Camera* camera, Vec3f direction, float delta_time);