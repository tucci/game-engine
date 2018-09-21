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
} Camera;


void init_camera_params(Camera* camera, float near, float far, float fov, float aspect_ratio);


typedef struct CameraManager {
	Camera* cameras;
	int count;
} CameraManager;
