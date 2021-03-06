#pragma once


#include "Math/Mat.h" 
#include "Math/Vec.h"
#include "ObjFile.h"
#include "Core/TextureData.h"

// TODO:The shader for now contains all the pipeline data.
// The purpose of this software renderer was to be as a learning experience and not actual real world use
struct SoftwareRendererShader {
	ObjModel* model;
	Texture2D* texture;
	Mat4x4f* transform;
	Vec4f pos[3];
	Vec2f uv[3];
	Vec3f normals[3];
};
