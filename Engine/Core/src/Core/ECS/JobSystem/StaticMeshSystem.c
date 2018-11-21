#pragma once


#include "Core/ECS/JobSystem/StaticMeshSystem.h"

StaticMeshID get_static_mesh(EntityManager* manager, Entity entity) {
	MapResult<u64> result = map_get(&manager->static_mesh_manger.id_map, entity.id);
	if (!result.found) {
		StaticMeshID id;
		id.id = -1;
		return id;
	}
	u64 index = result.value;
	StaticMeshID id;
	id.id = manager->static_mesh_manger.meshes[index];
	return id;
}

bool set_static_mesh(EntityManager* manager, Entity entity, StaticMeshID id) {
	
	MapResult<u64> result = map_get(&manager->static_mesh_manger.id_map, entity.id);
	if (!result.found) {
		return false;
	}
	
	u64 index = result.value;
	manager->static_mesh_manger.meshes[index] = id.id;
	return true;
}