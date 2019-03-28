#pragma once


#include "Math/Vec.h"

#include "Common/Map.h"
#include "Core/ECS/Entity.h"

struct LightID {
	u64 id;
};

enum class LightType {
	None, // Used when trying to get a light that doesnt exist
	DirectionalLight,
	PointLight,
};

struct DirectionalLight {
	Vec3f direction;
	Vec3f color;
	float intensity;
};

struct PointLight {
	Vec3f color;
};


struct Light {
	LightType type;
	union {
		DirectionalLight dir_light;
		PointLight point_light;
	};
	Light() {
		dir_light.direction = Vec3f(0, 0, 0);
		dir_light.color = Vec3f(1, 1, 1);
	};
};

struct LightManager {
	CompactMap<u64> id_map;
	Light* lights;
	u64 count;
};



void init_light_manager(LightManager* manager);
void destroy_light_manager(LightManager* manager);
bool entity_add_light_component(LightManager* manager, Entity entity);
bool entity_remove_light_component(LightManager* manager, Entity entity);