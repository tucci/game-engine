#pragma once


#include "Common/Arena.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Common/Map.h"




#define ECS_MEMORY MEGABYTES(40)

typedef enum ComponentType {
	ComponentType_None,
	ComponentType_Transform,
	ComponentType_Camera,
	ComponentType_StaticMesh,
} ComponentType;


#define NO_ENTITY_ID (uint64_t)0

typedef struct Entity {
	uint64_t id;
	Entity() {
		id = NO_ENTITY_ID;
	}
} Entity;



#define ENTITY_DEFAULT_START_SIZE 32
typedef struct EntityManager {
	
	TransformManager transforms;
	CameraManager cameras;
	StaticMeshManager static_meshs;
	

	Arena arena;
	StackAllocator stack_mem;

	Entity* entity_list;
	
} EntityManager;

void init_entity_manager(EntityManager* manager);
void destroy_entity_manager(EntityManager* manager);
Entity create_entity(EntityManager* manager);
void add_component(EntityManager* manager, Entity entity, ComponentType type);


void attach_child_entity(EntityManager* manager, Entity entity, Entity child);


Camera* get_camera(EntityManager* manager, Entity entity);
StaticMesh* get_static_mesh(EntityManager* manager, Entity entity);
void set_static_mesh(EntityManager* manager, Entity entity, StaticMesh* mesh);
