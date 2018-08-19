#pragma once


#include "StaticMesh.h"

#include "../Common/common_macros.h"


// TODO: instead of using a linerar allocator, we could probably use something better
void obj_to_static_mesh(ObjModel* obj, StaticMesh* static_mesh, LinearAllocator* memory) {

	int index_count = obj->face_count;
	static_mesh->index_count = index_count;
	static_mesh->indices = cast(Vec3i*)linear_alloc(memory, index_count * sizeof(Vec3i), 4);

	
	static_mesh->vertex_count = index_count * 3;
	static_mesh->pos = cast(Vec3f*)linear_alloc(memory, static_mesh->vertex_count * sizeof(Vec3f), 4);
	static_mesh->texcoords = cast(Vec2f*)linear_alloc(memory, static_mesh->vertex_count * sizeof(Vec2f), 4);





	for (int i = 0; i < index_count; i++) {
		static_mesh->indices[i] = (Vec3i) { 3 * i, 3 * i + 1, 3 * i + 2 };
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
