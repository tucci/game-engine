#pragma once




#include "Math/Vec.h"
#include "Core/TextureData.h"

struct MaterialID {
	u64 id;
	MaterialID() {
		id = 0;
	}
};

enum class MaterialShadingModel {
	Unknown,
	Phong,
	Lambert,
	// Our renderer is using the cook torrance model
	CookTorrance
};

enum TextureType {
	TextureType_None,
	TextureType_Albedo,
	TextureType_Normal,
	TextureType_Metal,
	TextureType_Roughness,
	TextureType_AO
};

struct Material {

	MaterialID id;

	MaterialShadingModel shading_model;	
	
	TextureID albedo;
	TextureID normal;
	TextureID metal;
	TextureID roughness;
	TextureID ao;


	//Texture2D* albedo;
	//Texture2D* normal;
	//Texture2D* metal;
	//Texture2D* roughness;
	//Texture2D* ao;

	// Used if no texture are assigned
	Vec3f albedo_color;
	Vec3f emissive_color;

	// 0 - 1 values
	float metallic_factor;
	float roughness_factor;
	float emissive_factor;
};

struct InternalMaterial {
	Material material;

	Texture2D* tx_albedo;
	Texture2D* tx_normal;
	Texture2D* tx_metal;
	Texture2D* tx_roughness;
	Texture2D* tx_ao;

	
};

inline void init_material_defaults(Material* material) {
	material->shading_model = MaterialShadingModel::CookTorrance;

	TextureID no_texture;
	
	// No texture ids
	material->albedo = no_texture;
	material->normal = no_texture;
	material->metal = no_texture;
	material->roughness = no_texture;
	material->ao = no_texture;

	material->albedo_color = Vec3f(0, 0, 0);
	material->emissive_color = Vec3f(0, 0, 0);

	material->metallic_factor = 0;
	material->roughness_factor = 0;
	material->emissive_factor = 0;
}

