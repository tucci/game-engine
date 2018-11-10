#pragma once


#include "Core/ECS/JobSystem/StaticMeshSystem.h"

StaticMeshID get_static_mesh(EntityManager* manager, Entity entity) {
	MapResult<uint64_t> result = map_get(&manager->static_mesh_manger.id_map, entity.id);
	if (!result.found) {
		StaticMeshID id;
		id.id = -1;
		return id;
	}
	uint64_t index = result.value;
	StaticMeshID id;
	id.id = manager->static_mesh_manger.meshes[index];
	return id;
}

bool set_static_mesh(EntityManager* manager, Entity entity, StaticMeshID id) {
	
	MapResult<uint64_t> result = map_get(&manager->static_mesh_manger.id_map, entity.id);
	if (!result.found) {
		return false;
	}
	
	uint64_t index = result.value;
	manager->static_mesh_manger.meshes[index] = id.id;
	return true;
}