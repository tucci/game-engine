#pragma once

#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/TransformSystem.h"


Camera* get_camera(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->camera_manager.id_map, entity.id);
	if (!result.found) return NULL;
	u64 index = result.value;
	return &manager->camera_manager.cameras[index];
}

void job_compute_camera_view_matrices(EntityManager* manager) {
	for (int i = 0; i < manager->camera_manager.count; i++) {
		Camera* cam = &manager->camera_manager.cameras[i];
		Entity e = manager->camera_manager.cameras[i].entity_ref;
		Vec3f cam_pos = position(manager, e);
		// Since camera is looking now -z, we want to move in that direction instead
		// so we need to flip the forward
		Vec3f cam_forward = -forward(manager, e);
		Vec3f cam_up = up(manager, e);

		
		cam->view_mat = look_at(cam_pos, cam_pos + cam_forward, cam_up);
	}
}
