#pragma once


#include "../../Math/Vec.h"

typedef struct Camera {
	Vec3f pos;

	Vec3f dir;
	Vec3f up;
	Vec3f right;

	Vec4f rotation;

	float near;
	float far;
	float fov;
	float aspect_ratio;

} Camera;


void init_camera(Camera* camera, Vec3f pos, Vec3f dir, Vec4f orientation, float near, float far, float fov, float aspect_ratio);
void init_camera_default(Camera* camera);
void set_camera_pos(Camera* camera, Vec3f pos);

void move_camera_in_direction(Camera* camera, Vec3f direction, float delta_time);