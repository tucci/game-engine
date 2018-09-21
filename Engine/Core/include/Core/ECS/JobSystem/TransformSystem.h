#pragma once

#include "Core/ECS/EntityManager.h"

void job_default_transforms(TransformManager* manager);

void job_update_basis_vectors(TransformManager* manager);

void job_compute_trs_matrices(TransformManager* manager);