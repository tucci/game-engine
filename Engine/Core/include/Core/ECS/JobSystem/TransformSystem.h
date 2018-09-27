#pragma once

#include "Core/ECS/EntityManager.h"




void job_update_basis_vectors(EntityManager* manager);

void job_compute_world_matrices(EntityManager* manager);

Mat4x4f* get_world_mat(EntityManager* manager, Entity entity);

Mat4x4f* get_local_mat(EntityManager* manager, Entity entity);

Vec3f position(EntityManager* manager, Entity entity);

void set_position(EntityManager* manager, Entity entity, Vec3f pos);

Vec3f get_scale(EntityManager* manager, Entity entity);

void set_scale(EntityManager* manager, Entity entity, Vec3f scale);

Quat rotation(EntityManager* manager, Entity entity);

void set_rotation(EntityManager* manager, Entity entity, Quat rotation);

Vec3f forward(EntityManager* manager, Entity entity);

Vec3f up(EntityManager* manager, Entity entity);

Vec3f right(EntityManager* manager, Entity entity);