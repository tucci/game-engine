#pragma once




#include "Core/ECS/Component/Camera.h"
#include "Core/ECS/Component/Transform.h"
#include "Core/ECS/Component/StaticMesh.h"



typedef enum ComponentType {
	ComponentType_None,
	ComponentType_Transform,
	ComponentType_Camera,
	ComponentType_StaticMesh,
};

typedef struct ComponentManager {
	TransformManager transform_manager;
	CameraManager camera_manager;
	StaticMeshManager static_mesh_manager;
} ComponentManager;


typedef struct Entity {
	int entity_id;
} Entity;



typedef struct EntityManager {
	enum EntityCountList {
		// TODO: the size should not be fixed. for now we are just testing
		size = 10
	};
	ComponentManager comp_manager;
	int last_entity_index = 0;
	LinearAllocator mem;

	// TODO: eventually use something better
	Entity entity_list[size];
	
} EntityManager;


Entity* create_entity(EntityManager* manager);
void add_component(EntityManager* manager, Entity* entity, ComponentType type);


void init_entity_manager(EntityManager* manager, void* parition_start, size_t partition_size);

Camera* get_camera(EntityManager* manager, Entity entity);
StaticMesh* get_static_mesh(EntityManager* manager, Entity entity);
Mat4x4f* get_model_mat(EntityManager* manager, Entity entity);
Vec3f position(EntityManager* manager, Entity entity);
void set_position(EntityManager* manager, Entity entity, Vec3f pos);

Vec3f get_scale(EntityManager* manager, Entity entity);
void set_scale(EntityManager* manager, Entity entity, Vec3f scale);

Quat rotation(EntityManager* manager, Entity entity);
void set_rotation(EntityManager* manager, Entity entity, Quat rotation);


Vec3f forward(EntityManager* manager, Entity entity);
Vec3f up(EntityManager* manager, Entity entity);
Vec3f right(EntityManager* manager, Entity entity);