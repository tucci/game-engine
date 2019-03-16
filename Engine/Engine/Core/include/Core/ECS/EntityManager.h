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

enum class ComponentType : u64 {
	None = 0x00,
	Transform = 1 << 0,
	Camera = 1 << 1,
	StaticMesh = 1 << 2,
	Light = 1 << 3,
	Render = 1 << 4,

	Count = 6,
};

inline ComponentType operator|(ComponentType a, ComponentType b) {
	return static_cast<ComponentType>(static_cast<int>(a) | static_cast<int>(b));
}

inline ComponentType operator&(ComponentType a, ComponentType b) {
	return static_cast<ComponentType>(static_cast<int>(a) & static_cast<int>(b));
}

inline ComponentType operator~(ComponentType a) {
	return static_cast<ComponentType>(~static_cast<int>(a));
}




struct ComponentManager {
	CompactMap<u64> id_map;
	ComponentType* types;
	u64 count;
};

static void init_component_manager(ComponentManager* cpm);
static void destroy_component_manager(ComponentManager* cpm);
static void create_component_flag(ComponentManager* cpm, Entity entity);
static void remove_component_flag(ComponentManager* cpm, Entity entity);
static void set_component_flag(ComponentManager* cpm, Entity entity, ComponentType component);
static void unset_component_flag(ComponentManager* cpm, Entity entity, ComponentType component);



struct EntityManager {
	
	ComponentManager component_manager;

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

Entity create_entity(EntityManager* manager, String name);
void destroy_entity(EntityManager* manager, Entity entity);

// TODO: these are attached to the transform manager. maybe we should move them there
void attach_child_entity(EntityManager* manager, Entity entity, Entity child);
u64 entity_count(EntityManager* manager);
Entity parent(EntityManager* manager, Entity entity);
Entity first_child(EntityManager* manager, Entity entity);
Entity next_sibling(EntityManager* manager, Entity entity);
u64 children_count(EntityManager* manager, Entity entity);





void add_component(EntityManager* manager, Entity entity, ComponentType component);
void remove_component(EntityManager* manager, Entity entity, ComponentType component);
bool has_component(EntityManager* manager, Entity entity, ComponentType component);



// quick helper function so we dont have to keep writing this below
u64 get_index_for_entity(EntityManager* manager, Entity entity, CompactMap<u64>* map);

