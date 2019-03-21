#pragma once




#include "Math/Vec.h"
#include "Core/TextureData.h"

struct MaterialID {
	u64 id;
};

enum class MaterialShadingModel {
	Unknown,
	Phong,
	Lambert,
	// Our renderer is using the cook torrance model
	CookTorrance
};

enum TextureType {
	None,
	Albedo,
	Normal,
	Metal,
	Roughness,
	AO
};

struct Material {

	MaterialID id;

	MaterialShadingModel shading_model;	
	Texture2D* albedo;
	Texture2D* normal;
	Texture2D* metal;
	Texture2D* roughness;
	Texture2D* ao;

	// Used if no texture are assigned
	Vec3f albedo_color;
	Vec3f emissive_color;

	// 0 - 1 values
	float metallic_factor;
	float roughness_factor;
	float emissive_factor;
};

static Material DEFAULT_MAT = {
	0,
	MaterialShadingModel::CookTorrance,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	Vec3f(1,0,0),
	Vec3f(0,0,0),
	0.0f,
	0.0f,
	0.0f
};

inline void init_material_defaults(Material* material) {
	material->shading_model = MaterialShadingModel::CookTorrance;

	// No texture ids
	material->albedo = NULL;
	material->normal = NULL;
	material->metal = NULL;
	material->roughness = NULL;
	material->ao = NULL;

	material->albedo_color = Vec3f(0, 0, 0);
	material->emissive_color = Vec3f(0, 0, 0);

	material->metallic_factor = 0;
	material->roughness_factor = 0;
	material->emissive_factor = 0;
}

static inline void update_material_with_texture_data(Material* material, TextureType type, TextureID id, Texture2D* texture, s32 layer) {

	// NEED TO FIGURE OUT HOW TO USE TEXTURE LAYERS
	switch (type) {
		case TextureType::Albedo: {
			material->albedo = texture;
			material->albedo->id = id;
			break;
		}
		case TextureType::Normal: {
			material->normal = texture;
			material->normal->id = id;
			break;
		}
		case TextureType::Metal: {
			material->metal = texture;
			material->metal->id = id;
			break;
		}
		case TextureType::Roughness: {
			material->roughness = texture;
			material->roughness->id = id;
			break;
		}
		case TextureType::AO: {
			material->ao = texture;
			material->ao->id = id;
			break;
		}
	}
}