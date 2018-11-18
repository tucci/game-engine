#pragma once

#include "Core/ECS/EntityManager.h"


Camera* get_camera(EntityManager* manager, Entity entity);

void job_compute_camera_view_matrices(EntityManager* manager);