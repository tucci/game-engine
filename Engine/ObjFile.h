#pragma once

#include "Math/Vec.h"

typedef struct ObjModel {
	Vec4f* verts;
	Vec2f* texcoords;
	Vec3f* normals;

	Vec3i* v_id;
	Vec3i* vt_id;
	Vec3i* vn_id;

	int vert_count;
	int texcoord_count;
	int normal_count;

	int face_count;
	int tex_indice_count;
	
} ObjModel;


// NOTE: these are deprecated, use obj_to_static_mesh in static mesh to load into our arenas
// This is used just for the legacy software renderer
void load_obj(const char* filename, ObjModel* model);
void free_obj(ObjModel* model);