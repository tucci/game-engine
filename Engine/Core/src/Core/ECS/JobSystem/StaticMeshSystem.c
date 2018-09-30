#pragma once


#include "Core/ECS/JobSystem/StaticMeshSystem.h"

StaticMesh* get_static_mesh(EntityManager* manager, Entity entity) {
	uint64_t index = map_get(&manager->static_meshs.id_map, entity.id);
	return &manager->static_meshs.meshes[index];
}

void set_static_mesh(EntityManager* manager, Entity entity, StaticMesh* mesh) {
	uint64_t index = map_get(&manager->static_meshs.id_map, entity.id);
	manager->static_meshs.meshes[index] = *mesh;
}