#pragma once


#include "../../Math/Vec.h"
#include "../../Math/Mat.h"

#include "../Transform.h"


typedef struct Camera {
	
	Transform transform;

	float near;
	float far;
	float fov;
	float aspect_ratio;

	Mat4x4f view_mat;
} Camera;


void init_camera(Camera* camera, Vec3f pos, Vec3f dir, float near, float far, float fov, float aspect_ratio);
void init_camera_default(Camera* camera);
void set_camera_pos(Camera* camera, Vec3f pos);

void move_camera_in_direction(Camera* camera, Vec3f direction, float delta_time);