#pragma once



#include "Asset/Asset.h"
#include "Math/Vec.h"

typedef enum MaterialShadingModel {
	MaterialShadingModel_Unknown,
	MaterialShadingModel_Phong,
	MaterialShadingModel_Lambert,
	// Our renderer is using the cook torrance model
	MaterialShadingModel_CookTorrance
}MaterialShadingModel;

typedef enum TextureType {
	TextureType_None,
	TextureType_Diffuse,
	TextureType_Specular,
	TextureType_Normal,
	TextureType_Metal,
	TextureType_Roughness,
	TextureType_AO
}TextureType;

typedef struct Material {
	MaterialShadingModel shading_model;

	TextureID albedo_texture;
	TextureID normal_texture;
	TextureID metal_texture;
	TextureID roughness_texture;
	TextureID ao_texture;

	// Used if no texture are assigned
	Vec3f albedo_color;
	Vec3f emissive_color;

	// 0 - 1 values
	float metallic_factor;
	float roughness_factor;
	float emissive_factor;
} Material;

inline void init_material_defaults(Material* material) {
	material->shading_model = MaterialShadingModel_CookTorrance;

	// No texture ids
	material->albedo_texture.id = 0;
	material->normal_texture.id = 0;
	material->metal_texture.id = 0;
	material->roughness_texture.id = 0;
	material->ao_texture.id = 0;

	material->albedo_color = Vec3f(0, 0, 0);
	material->emissive_color = Vec3f(0, 0, 0);

	material->metallic_factor = 0;
	material->roughness_factor = 0;
	material->emissive_factor = 0;
}

inline void set_texture_type_and_id(Material* material, TextureType type, TextureID id, s32 layer) {

	// NEED TO FIGURE OUT HOW TO USE TEXTURE LAYERS
	switch (type) {
		case TextureType_Diffuse: {
			material->albedo_texture = id;
			break;
		}
		case TextureType_Normal: {
			material->normal_texture = id;
			break;
		}
		case TextureType_Metal: {
			material->metal_texture = id;
			break;
		}
		case TextureType_Roughness: {
			material->roughness_texture = id;
			break;
		}
		case TextureType_AO: {
			material->ao_texture = id;
			break;
		}
	}
}