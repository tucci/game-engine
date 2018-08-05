#pragma once

#include "Math/Vec.h"
#include "ObjFile.h"


typedef struct StaticMesh {
	int vertex_count;
	int index_count;
	Vec3f* pos;
	Vec3f* normal;
	Vec2f* texcoords;
	Vec3i* indices;
} StaticMesh;


void inline convert_to_static_mesh(ObjModel* obj, StaticMesh* static_mesh) {
	
	int index_count = obj->face_count;
	static_mesh->index_count = index_count;
	static_mesh->indices = (Vec3i*)malloc(index_count * sizeof(Vec3i));


	static_mesh->vertex_count = index_count * 3;
	static_mesh->pos = (Vec3f*)malloc(static_mesh->vertex_count * sizeof(Vec3f));
	static_mesh->texcoords = (Vec2f*)malloc(static_mesh->vertex_count * sizeof(Vec2f));





	for (int i = 0; i < index_count; i++) {
		static_mesh->indices[i] = (Vec3i){3 * i, 3 * i + 1, 3 * i + 2 };
		Vec3i uv = obj->vt_id[i];
		Vec3i face = obj->v_id[i];
		

		static_mesh->pos[3 * i + 0] = obj->verts[face.x].xyz;
		static_mesh->pos[3 * i + 1] = obj->verts[face.y].xyz;
		static_mesh->pos[3 * i + 2] = obj->verts[face.z].xyz;

		static_mesh->texcoords[3 * i + 0] = obj->texcoords[uv.x];
		static_mesh->texcoords[3 * i + 1] = obj->texcoords[uv.y];
		static_mesh->texcoords[3 * i + 2] = obj->texcoords[uv.z];

	}

}


void inline free_static_mesh(StaticMesh* static_mesh) {
	if (static_mesh->pos != 0) {
		free(static_mesh->pos);
	}

	if (static_mesh->texcoords != 0) {
		free(static_mesh->texcoords);
	}

	if (static_mesh->indices != 0) {
		free(static_mesh->indices);
	}
}