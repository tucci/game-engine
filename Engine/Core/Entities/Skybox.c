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

	fill_texture_info(front, &skybox->front);
	fill_texture_info(back, &skybox->back);
	fill_texture_info(left, &skybox->left);
	fill_texture_info(right, &skybox->right);
	fill_texture_info(top, &skybox->top);
	fill_texture_info(bottom, &skybox->bottom);
	// NOTE: all the textures should be same width and height, and channel count
	int image_size = skybox->front.width * skybox->front.height * skybox->front.channels;

	skybox->front.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);
	skybox->back.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);
	skybox->left.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);
	skybox->right.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);
	skybox->top.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);
	skybox->bottom.data = cast(unsigned char*)linear_alloc(mem, image_size, 4);

	load_and_copyto_texture(front, &skybox->front, false);
	load_and_copyto_texture(back, &skybox->back, false);
	load_and_copyto_texture(left, &skybox->left, false);
	load_and_copyto_texture(right, &skybox->right, false);
	load_and_copyto_texture(top, &skybox->top, false);
	load_and_copyto_texture(bottom, &skybox->bottom, false);

	
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