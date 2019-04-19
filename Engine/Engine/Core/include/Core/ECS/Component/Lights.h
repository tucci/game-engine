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
		type = LightType::None;
		dir_light.direction = Vec3f(0, 0, 0);
		dir_light.color = Vec3f(1, 1, 1);
		dir_light.intensity = 1.0f;
	};
};

struct LightManager {
	CompactMap<u64> id_map;
	
	Entity* entitys;
	Light* lights;
	u64 enabled_count;
	u64 total_count;

	


};



void init_light_manager(LightManager* manager);
void destroy_light_manager(LightManager* manager);
bool entity_add_light_component(LightManager* manager, Entity entity);
bool entity_remove_light_component(LightManager* manager, Entity entity);
void enable_light_component(LightManager* manager, Entity entity, bool enabled);
bool is_light_component_enabled(LightManager* manager, Entity entity);