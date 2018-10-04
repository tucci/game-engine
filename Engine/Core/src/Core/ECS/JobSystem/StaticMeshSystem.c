#pragma once


#include "Core/ECS/JobSystem/StaticMeshSystem.h"

StaticMesh* get_static_mesh(EntityManager* manager, Entity entity) {
	MapResult<uint64_t> result = map_get(&manager->static_meshs.id_map, entity.id);
	if (!result.found) {
		return NULL;
	}
	uint64_t index = result.value;
	return &manager->static_meshs.meshes[index];
}

bool set_static_mesh(EntityManager* manager, Entity entity, StaticMesh* mesh) {
	
	MapResult<uint64_t> result = map_get(&manager->static_meshs.id_map, entity.id);
	if (!result.found) {
		return false;
	}
	uint64_t index = result.value;
	manager->static_meshs.meshes[index] = *mesh;
	return true;
}