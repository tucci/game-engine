#pragma once

#include "Core/Entities/Skybox.h"
#include "Core/Entities/Primitives.h"
#include "Common/common_macros.h"



void load_skybox(Skybox* skybox, LinearAllocator* mem,
	const char* front,
	const char* back,
	const char* left,
	const char* right,
	const char* top,
	const char* bottom) {

	

	load_texture(front, &skybox->front, mem, false);
	load_texture(back, &skybox->back, mem, false);
	load_texture(left, &skybox->left, mem, false);
	load_texture(right, &skybox->right, mem, false);
	load_texture(top, &skybox->top, mem, false);
	load_texture(bottom, &skybox->bottom, mem, false);

	
	make_cube(&skybox->cube, mem);
	/*skybox->cube.vertex_count = 36;
	

	const float box_size = 10.0f;

	skybox->cube.pos = cast(Vec3f*)linear_alloc(mem, skybox->cube.vertex_count * sizeof(Vec3f), 1);
	skybox->cube.pos[0]  = Vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[1]  = Vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[2]  = Vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[3]  = Vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[4]  = Vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[5]  = Vec3f(-box_size,  box_size, -box_size);

	skybox->cube.pos[6]  = Vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[7]  = Vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[8]  = Vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[9]  = Vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[10] = Vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[11] = Vec3f(-box_size, -box_size,  box_size);

	skybox->cube.pos[12] = Vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[13] = Vec3f( box_size, -box_size,  box_size);
	skybox->cube.pos[14] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[15] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[16] = Vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[17] = Vec3f( box_size, -box_size, -box_size);

	skybox->cube.pos[18] = Vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[19] = Vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[20] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[21] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[22] = Vec3f( box_size, -box_size,  box_size);
	skybox->cube.pos[23] = Vec3f(-box_size, -box_size,  box_size);

	skybox->cube.pos[24] = Vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[25] = Vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[26] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[27] = Vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[28] = Vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[29] = Vec3f(-box_size,  box_size, -box_size);

	skybox->cube.pos[30] = Vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[31] = Vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[32] = Vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[33] = Vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[34] = Vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[35] = Vec3f( box_size, -box_size,  box_size);*/
	

	
}

void load_hdr_skymap(HDR_SkyMap* map, LinearAllocator* mem, const char* filename) {
	load_hdr_texture(filename, &map->map, mem, true);
	make_cube(&map->cube, mem);
}