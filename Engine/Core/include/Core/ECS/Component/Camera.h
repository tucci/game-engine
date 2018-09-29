#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"


#include "Core/ECS/Component/Transform.h"
#include "Common/Map.h"


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


typedef struct CameraManager {
	CompactMap<uint64_t> id_map;
	Camera* cameras;
	uint64_t count;
} CameraManager;
