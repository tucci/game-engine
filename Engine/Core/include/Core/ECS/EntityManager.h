#pragma once

#include "Core/ECS/Entity.h"

#include "Common/Arena.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Lights.h"
#include "Common/Map.h"




#define ECS_MEMORY MEGABYTES(40)

typedef enum ComponentType {
	ComponentType_None,
	ComponentType_Transform,
	ComponentType_Camera,
	ComponentType_StaticMesh,
	ComponentType_Light,
} ComponentType;






typedef struct EntityManager {
	
	TransformManager transforms;
	CameraManager cameras;
	StaticMeshManager static_meshs;
	LightManager lights;
	

	Arena arena;
	StackAllocator stack_mem;

	Entity* entity_list;
	
} EntityManager;

void init_entity_manager(EntityManager* manager);
void destroy_entity_manager(EntityManager* manager);
Entity create_entity(EntityManager* manager);
void add_component(EntityManager* manager, Entity entity, ComponentType type);
void attach_child_entity(EntityManager* manager, Entity entity, Entity child);




