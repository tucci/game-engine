#pragma once

#include "Core/ECS/Component/Primitives.h"
#include "debug_macros.h"


void make_plane(StaticMesh* mesh, LinearAllocator* memory) {
	mesh->vertex_count = 4;
	mesh->index_count = 2;

	mesh->pos = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->pos[0] = Vec3f(0.5f, 0, 0.5f);
	mesh->pos[1] = Vec3f(0.5f, 0, -0.5f);
	mesh->pos[2] = Vec3f(-0.5f, 0, -0.5f);
	mesh->pos[3] = Vec3f(-0.5f, 0, 0.5f);

	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec2f), 4);
	mesh->texcoords[0] = Vec2f(1, 1);
	mesh->texcoords[1] = Vec2f(1, 0);
	mesh->texcoords[2] = Vec2f(0, 0);
	mesh->texcoords[3] = Vec2f(0, 1);


	mesh->color = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->color[0] = Vec3f(1, 1, 1);
	mesh->color[1] = Vec3f(1, 1, 1);
	mesh->color[2] = Vec3f(1, 1, 1);
	mesh->color[3] = Vec3f(1, 1, 1);

	mesh->normal = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->normal[0] = Vec3f(0, 1, 0);
	mesh->normal[1] = Vec3f(0, 1, 0);
	mesh->normal[2] = Vec3f(0, 1, 0);
	mesh->normal[3] = Vec3f(0, 1, 0);

	
	mesh->indices = cast(Vec3i*) linear_alloc(memory, mesh->index_count * sizeof(Vec3i), 4);
	mesh->indices[0] = Vec3i(0, 1, 2);
	mesh->indices[1] = Vec3i(0, 2, 3);

}


void make_cube(StaticMesh* mesh, LinearAllocator* memory) {
	mesh->vertex_count = 24;
	mesh->index_count = 12;

	mesh->pos = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
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



	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec2f), 1);

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




	mesh->color = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	// Bottom color
	mesh->color[0] = Vec3f(0, 1, 0);
	mesh->color[1] = Vec3f(0, 1, 0);
	mesh->color[2] = Vec3f(0, 1, 0);
	mesh->color[3] = Vec3f(0, 1, 0);

	// Top color
	mesh->color[5] = Vec3f(0, 1, 0);
	mesh->color[4] = Vec3f(0, 1, 0);
	mesh->color[6] = Vec3f(0, 1, 0);
	mesh->color[7] = Vec3f(0, 1, 0);


	// Right color
	mesh->color[8]  = Vec3f(1, 0, 0);
	mesh->color[9]  = Vec3f(1, 0, 0);
	mesh->color[10] = Vec3f(1, 0, 0);
	mesh->color[11] = Vec3f(1, 0, 0);


	// Left color
	mesh->color[12] = Vec3f(1, 0, 0);
	mesh->color[13] = Vec3f(1, 0, 0);
	mesh->color[14] = Vec3f(1, 0, 0);
	mesh->color[15] = Vec3f(1, 0, 0);

	// Front face
	mesh->color[16] = Vec3f(0, 0, 1);
	mesh->color[17] = Vec3f(0, 0, 1);
	mesh->color[18] = Vec3f(0, 0, 1);
	mesh->color[19] = Vec3f(0, 0, 1);

	// Back face
	mesh->color[20] = Vec3f(0, 0, 1);
	mesh->color[21] = Vec3f(0, 0, 1);
	mesh->color[22] = Vec3f(0, 0, 1);
	mesh->color[23] = Vec3f(0, 0, 1);


	mesh->normal = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
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


	mesh->indices = cast(Vec3i*) linear_alloc(memory, mesh->index_count * sizeof(Vec3i), 1);

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


void make_uv_sphere(StaticMesh* mesh, int lats, int longs, LinearAllocator* memory) {
	mesh->vertex_count = 4 * lats * longs;
	mesh->index_count = (2 * lats * longs) - (2 * longs);

	mesh->pos = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	mesh->color = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	mesh->normal = cast(Vec3f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec3f), 1);
	mesh->texcoords = cast(Vec2f*) linear_alloc(memory, mesh->vertex_count * sizeof(Vec2f), 1);
	mesh->indices = cast(Vec3i*) linear_alloc(memory, mesh->index_count * sizeof(Vec3i), 1);

	


	int vertex_count = 0;
	int index_count = 0;
	for (int t = 0; t < lats; t++) {
		// theta between [0, pi]
		float theta1 = (cast(float)t / lats) * PI;
		float theta2 = (cast(float)(t + 1) / lats) * PI;

		for (int p = 0; p < longs; p++) {
			// phi between [0, 2pi]
			float phi1 = (cast(float)p / longs) * 2 * PI;
			float phi2 = (cast(float)(p + 1) / longs) * 2 * PI;
			
			float sin_theta1 = sinf_(theta1);
			float cos_theta1 = cosf_(theta1);
			float sin_phi1 = sinf_(phi1);
			float cos_phi1  = cosf_(phi1);


			float sin_theta2 = sinf_(theta2);
			float cos_theta2 = cosf_(theta2);
			float sin_phi2 = sinf_(phi2);
			float cos_phi2 = cosf_(phi2);
			

			// Flip y, and z coordinates
			// Our y is up
			Vec3f v1 = Vec3f(0.5f * sin_theta1 * cos_phi1, 0.5f * cos_theta1, 0.5f * sin_theta1 * sin_phi1);
			Vec3f v2 = Vec3f(0.5f * sin_theta1 * cos_phi2, 0.5f * cos_theta1, 0.5f * sin_theta1 * sin_phi2);							   
			Vec3f v3 = Vec3f(0.5f * sin_theta2 * cos_phi2, 0.5f * cos_theta2, 0.5f * sin_theta2 * sin_phi2);
			Vec3f v4 = Vec3f(0.5f * sin_theta2 * cos_phi1, 0.5f * cos_theta2, 0.5f * sin_theta2 * sin_phi1);
			

			int vertex_index1 = vertex_count + 0;
			int vertex_index2 = vertex_count + 1;
			int vertex_index3 = vertex_count + 2;
			int vertex_index4 = vertex_count + 3;

			mesh->pos[vertex_index1] = v1;
			mesh->pos[vertex_index2] = v2;
			mesh->pos[vertex_index3] = v3;
			mesh->pos[vertex_index4] = v4;

			mesh->color[vertex_index1] = Vec3f(1,0, 0);
			mesh->color[vertex_index2] = Vec3f(1,0, 0);
			mesh->color[vertex_index3] = Vec3f(1,0, 0);
			mesh->color[vertex_index4] = Vec3f(1,0, 0);

			mesh->normal[vertex_index1] = v1;
			mesh->normal[vertex_index2] = v2;
			mesh->normal[vertex_index3] = v3;
			mesh->normal[vertex_index4] = v4;
			

			v1 = normalize(v1);
			v2 = normalize(v2);
			v3 = normalize(v3);
			v4 = normalize(v4);

			// TODO: this is a hack to fix the uv wrapping
			// Fix the division to work depedning if the longs are even or odd
			// This shouldnt be here once we fix the hack

			int l = longs % 2 == 0 ? longs : longs + 1;
			if (p == ((l)/2) - 1) {
				
				// To solve this, we should create duplicate vertices at the seam
				// This is because the texture tries to lerp between 0.9 and 0
				// Or look into cube mapped sphere
				//https://stackoverflow.com/questions/9511499/seam-issue-when-mapping-a-texture-to-a-sphere-in-opengl
				Vec2f t1 = Vec2f(1, 0.5f - (asinf_(v1.y) / PI));
				// t2 uses v1. it's ugly, and weird, and shouldnt work. but it does
				Vec2f t2 = Vec2f(0.5f + (atan2f_(v1.z, v1.x) / (2 * PI)), 0.5f - (asinf_(v2.y) / PI));
				Vec2f t3 = Vec2f(1, 0.5f - (asinf_(v3.y) / PI));
				Vec2f t4 = Vec2f(0.5f + (atan2f_(v4.z, v4.x) / (2 * PI)), 0.5f - (asinf_(v4.y) / PI));

				mesh->texcoords[vertex_index2] = t1;
				mesh->texcoords[vertex_index1] = t2;
				mesh->texcoords[vertex_index3] = t3;
				mesh->texcoords[vertex_index4] = t4;
			} else {
				mesh->texcoords[vertex_index2] = Vec2f(0.5f + (atan2f_(v2.z, v2.x) / (2 * PI)), 0.5f - (asinf_(v2.y) / PI));
				mesh->texcoords[vertex_index1] = Vec2f(0.5f + (atan2f_(v1.z, v1.x) / (2 * PI)), 0.5f - (asinf_(v1.y) / PI));
				mesh->texcoords[vertex_index3] = Vec2f(0.5f + (atan2f_(v3.z, v3.x) / (2 * PI)), 0.5f - (asinf_(v3.y) / PI));
				mesh->texcoords[vertex_index4] = Vec2f(0.5f + (atan2f_(v4.z, v4.x) / (2 * PI)), 0.5f - (asinf_(v4.y) / PI));
			}
			

			

			
			


			if (t == 0) {
				// Top cap is just a triangle
				mesh->indices[index_count] = Vec3i(vertex_index1, vertex_index3, vertex_index4);
				index_count++;
			}
			else if (t + 1 == lats) {
				// Bottom cap
				mesh->indices[index_count] = Vec3i(vertex_index3, vertex_index1, vertex_index2);
				index_count++;
			} else {
				// Other outside caps
				mesh->indices[index_count] = Vec3i(vertex_index1, vertex_index2, vertex_index4);
				mesh->indices[index_count + 1] = Vec3i(vertex_index2, vertex_index3, vertex_index4);
				index_count += 2;
			}

			vertex_count += 4;

		}
	}

	assert(mesh->index_count == index_count);
	assert(mesh->vertex_count == vertex_count);

	



	
}

