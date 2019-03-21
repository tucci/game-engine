#pragma once

#include "Core/ECS/Component/Primitives.h"
#include "debug_macros.h"



void make_cube(StaticMesh* mesh, StackAllocator* memory) {
	mesh->vertex_count = 24;
	mesh->index_count = 12;

	void* mem_block = stack_alloc(memory,
		mesh->index_count * sizeof(Vec3i)  // indices
		+ mesh->vertex_count * sizeof(Vec3f) // pos
		+ mesh->vertex_count * sizeof(Vec3f) // normals
		+ mesh->vertex_count * sizeof(Vec2f) // texcoords
		, 4);

	mesh->indices = cast(Vec3i*) mem_block;
	mesh->pos = cast(Vec3f*) (cast(char*)mem_block + (mesh->index_count * sizeof(Vec3i))); // + offset of indices block
	mesh->normal = cast(Vec3f*)(cast(char*)mesh->pos + (mesh->vertex_count * sizeof(Vec3f))); // + offset of pos block
	mesh->texcoords = cast(Vec2f*)(cast(char*)mesh->normal + (mesh->vertex_count * sizeof(Vec3f))); // + offset of normal block

	
	// Bottom Face
	mesh->pos[0] = Vec3f( 0.5f, -0.5f,  0.5f);
	mesh->pos[1] = Vec3f( 0.5f, -0.5f, -0.5f);
	mesh->pos[2] = Vec3f(-0.5f, -0.5f,  -0.5f);
	mesh->pos[3] = Vec3f(-0.5f, -0.5f,   0.5f);


	// Top Face
	mesh->pos[4] = Vec3f(0.5f, 0.5f, 0.5f);
	mesh->pos[5] = Vec3f(0.5f, 0.5f, -0.5f);
	mesh->pos[6] = Vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[7] = Vec3f(-0.5f, 0.5f,  0.5f);


	// Right face
	mesh->pos[8] = Vec3f(0.5f, 0.5f, 0.5f);
	mesh->pos[9] = Vec3f(0.5f, 0.5f, -0.5f);
	mesh->pos[10] = Vec3f(0.5f, -0.5f, -0.5f);
	mesh->pos[11] = Vec3f(0.5f, -0.5f, 0.5f);


	// Left face
	mesh->pos[12] = Vec3f(-0.5f, 0.5f, 0.5f);
	mesh->pos[13] = Vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[14] = Vec3f(-0.5f, -0.5f, -0.5f);
	mesh->pos[15] = Vec3f(-0.5f, -0.5f, 0.5f);

	// Front face
	mesh->pos[16] = Vec3f(-0.5f, 0.5f, 0.5f);
	mesh->pos[17] = Vec3f(0.5f, 0.5f, 0.5f);
	mesh->pos[18] = Vec3f(-0.5f, -0.5f, 0.5f);
	mesh->pos[19] = Vec3f(0.5f, -0.5f, 0.5f);

	// Back face
	mesh->pos[20] = Vec3f(-0.5f, 0.5f, -0.5f);
	mesh->pos[21] = Vec3f(0.5f, 0.5f, -0.5f);
	mesh->pos[22] = Vec3f(-0.5f, -0.5f, -0.5f);
	mesh->pos[23] = Vec3f(0.5f, -0.5f, -0.5f);



	

	// Bottom uv
	mesh->texcoords[0] = Vec2f(1, 1);
	mesh->texcoords[1] = Vec2f(1, 0);
	mesh->texcoords[2] = Vec2f(0, 0);
	mesh->texcoords[3] = Vec2f(0, 1);

	// Top uv
	mesh->texcoords[5] = Vec2f(1, 1);
	mesh->texcoords[4] = Vec2f(1, 0);
	mesh->texcoords[6] = Vec2f(0, 1);
	mesh->texcoords[7] = Vec2f(0, 0);


	// Right uv
	mesh->texcoords[8] = Vec2f(1, 1);
	mesh->texcoords[9] = Vec2f(0, 1);
	mesh->texcoords[10] = Vec2f(0, 0);
	mesh->texcoords[11] = Vec2f(1, 0);


	// Left uv
	mesh->texcoords[12] = Vec2f(0, 1);
	mesh->texcoords[13] = Vec2f(1, 1);
	mesh->texcoords[14] = Vec2f(1, 0);
	mesh->texcoords[15] = Vec2f(0, 0);

	// Front uv
	mesh->texcoords[16] = Vec2f(0, 1);
	mesh->texcoords[17] = Vec2f(1, 1);
	mesh->texcoords[18] = Vec2f(0, 0);
	mesh->texcoords[19] = Vec2f(1, 0);

	// Back uv
	mesh->texcoords[20] = Vec2f(0, 1);
	mesh->texcoords[21] = Vec2f(1, 1);
	mesh->texcoords[22] = Vec2f(0, 0);
	mesh->texcoords[23] = Vec2f(1, 0);



	
	// Bottom normal
	mesh->normal[0] = Vec3f(0, -1, 0);
	mesh->normal[1] = Vec3f(0, -1, 0);
	mesh->normal[2] = Vec3f(0, -1, 0);
	mesh->normal[3] = Vec3f(0, -1, 0);

	// Top normal
	mesh->normal[5] = Vec3f(0, 1, 0);
	mesh->normal[4] = Vec3f(0, 1, 0);
	mesh->normal[6] = Vec3f(0, 1, 0);
	mesh->normal[7] = Vec3f(0, 1, 0);


	// Right normal
	mesh->normal[8] = Vec3f(1, 0, 0);
	mesh->normal[9] = Vec3f(1, 0, 0);
	mesh->normal[10] = Vec3f(1, 0, 0);
	mesh->normal[11] = Vec3f(1, 0, 0);


	// Left normal
	mesh->normal[12] = Vec3f(-1, 0, 0);
	mesh->normal[13] = Vec3f(-1, 0, 0);
	mesh->normal[14] = Vec3f(-1, 0, 0);
	mesh->normal[15] = Vec3f(-1, 0, 0);

	// Front normal
	mesh->normal[16] = Vec3f(0, 0, 1);
	mesh->normal[17] = Vec3f(0, 0, 1);
	mesh->normal[18] = Vec3f(0, 0, 1);
	mesh->normal[19] = Vec3f(0, 0, 1);

	// Back normal
	mesh->normal[20] = Vec3f(0, 0, -1);
	mesh->normal[21] = Vec3f(0, 0, -1);
	mesh->normal[22] = Vec3f(0, 0, -1);
	mesh->normal[23] = Vec3f(0, 0, -1);


	

	// Bottom face
	mesh->indices[0] = Vec3i(2, 1, 0);
	mesh->indices[1] = Vec3i(3, 2, 0);

	// Top face
	mesh->indices[2] = Vec3i(4, 5, 6);
	mesh->indices[3] = Vec3i(4, 6, 7);


	// Right face
	mesh->indices[4] = Vec3i(10, 9, 8);
	mesh->indices[5] = Vec3i(10, 8, 11);


	// Left face
	mesh->indices[6] = Vec3i(13, 14, 15);
	mesh->indices[7] = Vec3i(13, 15, 12);


	// Front face
	mesh->indices[8] = Vec3i(16, 18, 19);
	mesh->indices[9] = Vec3i(16, 19, 17);

	// Back face
	mesh->indices[10] = Vec3i(23, 22, 20);
	mesh->indices[11] = Vec3i(21, 23, 20);

}


