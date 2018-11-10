#pragma once

#include "Math/Vec.h"
#include "Math/Mat.h"



#include "Common/Map.h"

#include "Core/ECS/Entity.h"

struct EntityManager;


typedef struct Camera {
	
	//Transform transform;
	Entity entity_ref;
	float near;
	float far;
	float fov;
	float aspect_ratio;
	Mat4x4f view_mat;
	Camera(Entity ref) {
		entity_ref = ref;
		near = 0.0001f;
		far = 100.0f;
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



void init_camera_manager(CameraManager* manager);
void destroy_camera_manager(CameraManager* manager);
void entity_add_camera_component(CameraManager* manager, Entity entity);
void entity_remove_camera_component(CameraManager* manager, Entity entity);
