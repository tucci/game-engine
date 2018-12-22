#pragma once


#include "Math/Vec.h"

#include "Common/Map.h"
#include "Core/ECS/Entity.h"

typedef struct LightID {
	u64 id;
} LightID;

typedef enum LightType {
	LightType_None, // Used when trying to get a light that doesnt exist
	LightType_DirectionalLight,
	LightType_PointLight,
} LightType;

typedef struct DirectionalLight {
	Vec3f direction;
	Vec3f color;
} DirectionalLight;

typedef struct PointLight {
	Vec3f position;
	Vec3f color;
} PointLight;


typedef struct Light {
	LightType type;
	union {
		DirectionalLight dir_light;
		PointLight point_light;
	};
	Light() {
		dir_light.direction = Vec3f(0, 0, 0);
		dir_light.color = Vec3f(1, 1, 1);
	};
} Light;

typedef struct LightManager {
	CompactMap<u64> id_map;
	Light* lights;
	u64 count;
} LightManager;



void init_light_manager(LightManager* manager);
void destroy_light_manager(LightManager* manager);
void entity_add_light_component(LightManager* manager, Entity entity);
void entity_remove_light_component(LightManager* manager, Entity entity);