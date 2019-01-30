#pragma once

#include "Core/ECS/Entity.h"

#include "Asset/Asset.h"
#include "Common/Arena.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Lights.h"
#include "Core/ECS/Component/RenderComponent.h"
#include "Common/Map.h"




#define ECS_MEMORY MEGABYTES(40)

enum class ComponentType {
	None,
	Transform,
	Camera,
	StaticMesh,
	Light,
	Render,

	Count
};






struct EntityManager {
	
	TransformManager transform_manager;
	CameraManager camera_manager;
	StaticMeshManager static_mesh_manger;
	LightManager light_manager;
	RenderManager render_manager;




	Arena arena;
	StackAllocator stack_mem;

	u64 entitys_created;
	u64 entity_count;
	Entity* entity_list;
	CompactMap<u64> entity_index_map;
	
	
};

void init_entity_manager(EntityManager* manager);
void destroy_entity_manager(EntityManager* manager);

Entity create_entity(EntityManager* manager);
void destroy_entity(EntityManager* manager, Entity entity);
void attach_child_entity(EntityManager* manager, Entity entity, Entity child);

void add_component(EntityManager* manager, Entity entity, ComponentType type);
void remove_component(EntityManager* manager, Entity entity, ComponentType type);



// quick helper function so we dont have to keep writing this below
u64 get_index_for_entity(EntityManager* manager, Entity entity, CompactMap<u64>* map);

