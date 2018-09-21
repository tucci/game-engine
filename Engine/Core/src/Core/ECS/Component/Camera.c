#pragma once

#include "Core/ECS/Component/Camera.h"

#include "Common/common_macros.h"



void init_camera_params(Camera* camera,
	float near,
	float far,
	float fov,
	float aspect_ratio) {

	camera->near = near;
	camera->far = far;

	camera->fov = fov;
	camera->aspect_ratio = aspect_ratio;
}

