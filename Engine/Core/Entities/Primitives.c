#pragma once

#include "../StaticMesh.h"
#include "../../Common/common_macros.h"


void inline make_plane(StaticMesh* mesh, LinearAllocator* memory) {
	mesh->vertex_count = 4;
	mesh->index_count = 2;

	mesh->pos = cast(Vec3f*) linear_alloc(memory, 4 * sizeof(Vec3f), 4);

	mesh->pos[0] = make_vec3f(0.5f, 0, 0.5f);
	mesh->pos[1] = make_vec3f(0.5f, 0, -0.5f);
	mesh->pos[2] = make_vec3f(-0.5f, 0, -0.5f);
	mesh->pos[3] = make_vec3f(-0.5f, 0, 0.5f);

	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, 4 * sizeof(Vec2f), 4);


	mesh->texcoords[0] = make_vec2f(1, 1);
	mesh->texcoords[1] = make_vec2f(1, 0);
	mesh->texcoords[2] = make_vec2f(0, 0);
	mesh->texcoords[3] = make_vec2f(0, 1);

	
	mesh->indices = cast(Vec3i*) linear_alloc(memory, 2 * sizeof(Vec3i), 4);

	mesh->indices[0] = make_vec3i(0, 1, 2);
	mesh->indices[1] = make_vec3i(0, 2, 3);

}

