#pragma once




#include "Math/Vec.h"
#include "Core/TextureData.h"

typedef struct MaterialID {
	u64 id;
} MaterialID;

typedef enum MaterialShadingModel {
	MaterialShadingModel_Unknown,
	MaterialShadingModel_Phong,
	MaterialShadingModel_Lambert,
	// Our renderer is using the cook torrance model
	MaterialShadingModel_CookTorrance
} MaterialShadingModel;

typedef enum TextureType {
	TextureType_None,
	TextureType_Albedo,
	TextureType_Normal,
	TextureType_Metal,
	TextureType_Roughness,
	TextureType_AO
} TextureType;

typedef struct Material {

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
} Material;

static Material DEFAULT_MAT = {
	0,
	MaterialShadingModel_CookTorrance,
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
	material->shading_model = MaterialShadingModel_CookTorrance;

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
		case TextureType_Albedo: {
			material->albedo = texture;
			material->albedo->id = id;
			break;
		}
		case TextureType_Normal: {
			material->normal = texture;
			material->normal->id = id;
			break;
		}
		case TextureType_Metal: {
			material->metal = texture;
			material->metal->id = id;
			break;
		}
		case TextureType_Roughness: {
			material->roughness = texture;
			material->roughness->id = id;
			break;
		}
		case TextureType_AO: {
			material->ao = texture;
			material->ao->id = id;
			break;
		}
	}
}