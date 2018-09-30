#pragma once


#include "Math/Vec.h"

#include "Common/Map.h"
#include "Core/ECS/Entity.h"

typedef enum LightType {
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
	CompactMap<uint64_t> id_map;
	Light* lights;
	uint64_t count;
} LightManager;



void init_light_manager(LightManager* manager);
void destroy_light_manager(LightManager* manager);
void entity_add_light_component(LightManager* manager, Entity entity);