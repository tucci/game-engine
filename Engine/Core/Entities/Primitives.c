#pragma once

#include "Primitives.h"


void make_plane(StaticMesh* mesh, LinearAllocator* memory) {
	mesh->vertex_count = 4;
	mesh->index_count = 2;

	mesh->pos = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->pos[0] = make_vec3f(0.5f, 0, 0.5f);
	mesh->pos[1] = make_vec3f(0.5f, 0, -0.5f);
	mesh->pos[2] = make_vec3f(-0.5f, 0, -0.5f);
	mesh->pos[3] = make_vec3f(-0.5f, 0, 0.5f);

	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec2f), 4);
	mesh->texcoords[0] = make_vec2f(1, 1);
	mesh->texcoords[1] = make_vec2f(1, 0);
	mesh->texcoords[2] = make_vec2f(0, 0);
	mesh->texcoords[3] = make_vec2f(0, 1);


	mesh->color = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->color[0] = make_vec3f(1, 0, 0);
	mesh->color[1] = make_vec3f(0, 1, 0);
	mesh->color[2] = make_vec3f(0, 0, 1);
	mesh->color[3] = make_vec3f(1, 1, 1);

	
	mesh->indices = cast(Vec3i*) linear_alloc(memory, mesh->index_count * sizeof(Vec3i), 4);
	mesh->indices[0] = make_vec3i(0, 1, 2);
	mesh->indices[1] = make_vec3i(0, 2, 3);

}


void make_cube(StaticMesh* mesh, LinearAllocator* memory) {
	mesh->vertex_count = 24;
	mesh->index_count = 12;

	mesh->pos = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	// Bottom Face
	mesh->pos[0] = make_vec3f( 0.5f, -0.5f,  0.5f);
	mesh->pos[1] = make_vec3f( 0.5f, -0.5f, -0.5f);
	mesh->pos[2] = make_vec3f(-0.5f, -0.5f,  -0.5f);
	mesh->pos[3] = make_vec3f(-0.5f, -0.5f,   0.5f);


	// Top Face
	mesh->pos[4] = make_vec3f( 0.5f, 0.5f,  0.5f);
	mesh->pos[5] = make_vec3f( 0.5f, 0.5f, -0.5f);
	mesh->pos[6] = make_vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[7] = make_vec3f(-0.5f, 0.5f,  0.5f);


	// Right face
	mesh->pos[8] = make_vec3f(0.5f, 0.5f, 0.5f);
	mesh->pos[9] = make_vec3f(0.5f, 0.5f, -0.5f);
	mesh->pos[10] = make_vec3f(0.5f, -0.5f, -0.5f);
	mesh->pos[11] = make_vec3f(0.5f, -0.5f, 0.5f);


	// Left face
	mesh->pos[12] = make_vec3f(-0.5f, 0.5f, 0.5f);
	mesh->pos[13] = make_vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[14] = make_vec3f(-0.5f, -0.5f, -0.5f);
	mesh->pos[15] = make_vec3f(-0.5f, -0.5f, 0.5f);

	// Front face
	mesh->pos[16] = make_vec3f(-0.5f, 0.5f, 0.5f);
	mesh->pos[17] = make_vec3f(0.5f, 0.5f, 0.5f);
	mesh->pos[18] = make_vec3f(-0.5f, -0.5f, 0.5f);
	mesh->pos[19] = make_vec3f(0.5f, -0.5f, 0.5f);

	// Back face
	mesh->pos[20] = make_vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[21] = make_vec3f(0.5f, 0.5f, -0.5f);
	mesh->pos[22] = make_vec3f(-0.5f, -0.5f, -0.5f);
	mesh->pos[23] = make_vec3f(0.5f, -0.5f, -0.5f);



	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec2f), 1);

	// Bottom uv
	mesh->texcoords[0] = make_vec2f(1, 1);
	mesh->texcoords[1] = make_vec2f(1, 0);
	mesh->texcoords[2] = make_vec2f(0, 0);
	mesh->texcoords[3] = make_vec2f(0, 1);

	// Top uv
	mesh->texcoords[5] = make_vec2f(1, 1);
	mesh->texcoords[4] = make_vec2f(1, 0);
	mesh->texcoords[6] = make_vec2f(0, 1);
	mesh->texcoords[7] = make_vec2f(0, 0);


	// Right uv
	mesh->texcoords[8] = make_vec2f(1, 1);
	mesh->texcoords[9] = make_vec2f(0, 1);
	mesh->texcoords[10] = make_vec2f(0, 0);
	mesh->texcoords[11] = make_vec2f(1, 0);


	// Left uv
	mesh->texcoords[12] = make_vec2f(0, 1);
	mesh->texcoords[13] = make_vec2f(1, 1);
	mesh->texcoords[14] = make_vec2f(1, 0);
	mesh->texcoords[15] = make_vec2f(0, 0);

	// Front uv
	mesh->texcoords[16] = make_vec2f(0, 1);
	mesh->texcoords[17] = make_vec2f(1, 1);
	mesh->texcoords[18] = make_vec2f(0, 0);
	mesh->texcoords[19] = make_vec2f(1, 0);

	// Back uv
	mesh->texcoords[20] = make_vec2f(0, 1);
	mesh->texcoords[21] = make_vec2f(1, 1);
	mesh->texcoords[22] = make_vec2f(0, 0);
	mesh->texcoords[23] = make_vec2f(1, 0);




	mesh->color = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	// Bottom color
	mesh->color[0] = make_vec3f(0, 1, 0);
	mesh->color[1] = make_vec3f(0, 1, 0);
	mesh->color[2] = make_vec3f(0, 1, 0);
	mesh->color[3] = make_vec3f(0, 1, 0);

	// Top color
	mesh->color[5] = make_vec3f(0, 1, 0);
	mesh->color[4] = make_vec3f(0, 1, 0);
	mesh->color[6] = make_vec3f(0, 1, 0);
	mesh->color[7] = make_vec3f(0, 1, 0);


	// Right color
	mesh->color[8]  = make_vec3f(1, 0, 0);
	mesh->color[9]  = make_vec3f(1, 0, 0);
	mesh->color[10] = make_vec3f(1, 0, 0);
	mesh->color[11] = make_vec3f(1, 0, 0);


	// Left color
	mesh->color[12] = make_vec3f(1, 0, 0);
	mesh->color[13] = make_vec3f(1, 0, 0);
	mesh->color[14] = make_vec3f(1, 0, 0);
	mesh->color[15] = make_vec3f(1, 0, 0);

	// Front face
	mesh->color[16] = make_vec3f(0, 0, 1);
	mesh->color[17] = make_vec3f(0, 0, 1);
	mesh->color[18] = make_vec3f(0, 0, 1);
	mesh->color[19] = make_vec3f(0, 0, 1);

	// Back face
	mesh->color[20] = make_vec3f(0, 0, 1);
	mesh->color[21] = make_vec3f(0, 0, 1);
	mesh->color[22] = make_vec3f(0, 0, 1);
	mesh->color[23] = make_vec3f(0, 0, 1);


	mesh->indices = cast(Vec3i*) linear_alloc(memory, mesh->index_count * sizeof(Vec3i), 1);

	// Bottom face
	mesh->indices[0] = make_vec3i(2, 1, 0);
	mesh->indices[1] = make_vec3i(3, 2, 0);

	// Top face
	mesh->indices[2] = make_vec3i(4, 5, 6);
	mesh->indices[3] = make_vec3i(4, 6, 7);


	// Right face
	mesh->indices[4] = make_vec3i(10, 9, 8);
	mesh->indices[5] = make_vec3i(10, 8, 11);


	// Left face
	mesh->indices[6] = make_vec3i(13, 14, 15);
	mesh->indices[7] = make_vec3i(13, 15, 12);


	// Front face
	mesh->indices[8] = make_vec3i(16, 18, 19);
	mesh->indices[9] = make_vec3i(16, 19, 17);

	// Back face
	mesh->indices[10] = make_vec3i(23, 22, 20);
	mesh->indices[11] = make_vec3i(21, 23, 20);

}

