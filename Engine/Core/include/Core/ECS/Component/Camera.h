#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"


#include "Core/ECS/Component/Transform.h"


typedef struct Camera {
	
	//Transform transform;

	float near;
	float far;
	float fov;
	float aspect_ratio;
	Mat4x4f view_mat;
	Camera() {
		near = 0.1f;
		far = 10.0f;
		fov = 90.0f;
		aspect_ratio = 1.0f;
		view_mat = Mat4x4f();
	}
} Camera;


void init_camera_params(Camera* camera, float near, float far, float fov, float aspect_ratio);

#include <unordered_map>
typedef struct CameraManager {
	std::unordered_map<int, int> id_map;
	Camera* cameras;
	int count;
} CameraManager;
