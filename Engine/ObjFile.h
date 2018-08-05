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



void load_obj(const char* filename, ObjModel* model);
void free_obj(ObjModel* model);