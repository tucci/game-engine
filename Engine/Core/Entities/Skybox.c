#pragma once

#include "Skybox.h"
#include "../../Common/common_macros.h"
#include "../Entities/Primitives.h"


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
	skybox->cube.pos[0]  = make_vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[1]  = make_vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[2]  = make_vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[3]  = make_vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[4]  = make_vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[5]  = make_vec3f(-box_size,  box_size, -box_size);

	skybox->cube.pos[6]  = make_vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[7]  = make_vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[8]  = make_vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[9]  = make_vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[10] = make_vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[11] = make_vec3f(-box_size, -box_size,  box_size);

	skybox->cube.pos[12] = make_vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[13] = make_vec3f( box_size, -box_size,  box_size);
	skybox->cube.pos[14] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[15] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[16] = make_vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[17] = make_vec3f( box_size, -box_size, -box_size);

	skybox->cube.pos[18] = make_vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[19] = make_vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[20] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[21] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[22] = make_vec3f( box_size, -box_size,  box_size);
	skybox->cube.pos[23] = make_vec3f(-box_size, -box_size,  box_size);

	skybox->cube.pos[24] = make_vec3f(-box_size,  box_size, -box_size);
	skybox->cube.pos[25] = make_vec3f( box_size,  box_size, -box_size);
	skybox->cube.pos[26] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[27] = make_vec3f( box_size,  box_size,  box_size);
	skybox->cube.pos[28] = make_vec3f(-box_size,  box_size,  box_size);
	skybox->cube.pos[29] = make_vec3f(-box_size,  box_size, -box_size);

	skybox->cube.pos[30] = make_vec3f(-box_size, -box_size, -box_size);
	skybox->cube.pos[31] = make_vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[32] = make_vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[33] = make_vec3f( box_size, -box_size, -box_size);
	skybox->cube.pos[34] = make_vec3f(-box_size, -box_size,  box_size);
	skybox->cube.pos[35] = make_vec3f( box_size, -box_size,  box_size);*/
	

	
}

void load_hdr_skymap(HDR_SkyMap* map, LinearAllocator* mem, const char* filename) {
	load_hdr_texture(filename, &map->map, mem, true);
	make_cube(&map->cube, mem);
}