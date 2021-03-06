#pragma once

#include "Core/ECS/Entity.h"

#include "Asset/Asset.h"
#include "Common/Arena.h"
#include "Common/StackAllocator.h"

#include "Core/ECS/Component/MetaInfo.h"
#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"
#include "Core/ECS/Component/Lights.h"
#include "Core/ECS/Component/RenderComponent.h"
#include "Common/Map.h"




#define ECS_MEMORY MEGABYTES(40)

enum class ComponentType : u64 {
	None = 0x00,
	MetaInfo = 1 << 0,
	Transform = 1 << 1,
	Camera = 1 << 2,
	StaticMesh = 1 << 3,
	Light = 1 << 4,
	Render = 1 << 5,

	Count = 6,
};


// Turn on a component
inline ComponentType operator|(ComponentType a, ComponentType b) {
	return static_cast<ComponentType>(static_cast<u64>(a) | static_cast<u64>(b));
}

// Check if component is on
inline ComponentType operator&(ComponentType a, ComponentType b) {
	return static_cast<ComponentType>(static_cast<u64>(a) & static_cast<u64>(b));
}

// Remove component
inline ComponentType operator~(ComponentType a) {
	return static_cast<ComponentType>(~static_cast<u64>(a));
}




struct ComponentTypesManager {
	CompactMap<u64> id_map;
	ComponentType* types;
	u64 count;
};

static void init_component_manager(ComponentTypesManager* cpm);
static void destroy_component_manager(ComponentTypesManager* cpm);
static void create_component_flag(ComponentTypesManager* cpm, Entity entity);
static void remove_component_flag(ComponentTypesManager* cpm, Entity entity);
static void set_component_flag(ComponentTypesManager* cpm, Entity entity, ComponentType component);
static void unset_component_flag(ComponentTypesManager* cpm, Entity entity, ComponentType component);



struct EntityManager {
	
	ComponentTypesManager component_types_manager;

	MetaInfoManager meta_manager;
	TransformManager transform_manager;
	CameraManager camera_manager;
	StaticMeshManager static_mesh_manger;
	LightManager light_manager;
	RenderManager render_manager;




	
	Arena arena;
	StackAllocator stack_mem;

	u64 entitys_created;
	u64 entity_count;
	Entity root;
	Entity* entity_list;
	CompactMap<u64> entity_index_map;
	
	
};

void init_entity_manager(EntityManager* manager);
void destroy_entity_manager(EntityManager* manager);

Entity create_entity(EntityManager* manager, String name);
bool destroy_entity(EntityManager* manager, Entity entity);
void enable_entity(EntityManager* manager, Entity entity, bool enabled);



// TODO: these are attached to the transform manager. maybe we should move them there
void attach_child_entity(EntityManager* manager, Entity entity, Entity child);
u64 entity_count(EntityManager* manager);
Entity parent(EntityManager* manager, Entity entity);
Entity first_child(EntityManager* manager, Entity entity);
Entity next_sibling(EntityManager* manager, Entity entity);
u64 children_count(EntityManager* manager, Entity entity);





bool add_component(EntityManager* manager, Entity entity, ComponentType component);
// Internal remove component that allows the entity manager to destory any component
static bool em_remove_component(EntityManager* manager, Entity entity, ComponentType component);
// Exposed remove component function can delete most components except internal components such as meta/transform
bool remove_component(EntityManager* manager, Entity entity, ComponentType component);
bool has_component(EntityManager* manager, Entity entity, ComponentType component);
void enable_component(EntityManager* manager, Entity entity, ComponentType component, bool enabled);
bool is_component_enabled(EntityManager* manager, Entity entity, ComponentType component);




// quick helper function so we dont have to keep writing this below
u64 get_index_for_entity(EntityManager* manager, Entity entity, CompactMap<u64>* map);


//Entity import_asset_scene(EntityManager* manager, SceneID id);


