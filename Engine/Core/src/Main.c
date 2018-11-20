#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"


#include "debug_macros.h"

#define ZLIB_WINAPI








int main(int argc, char* argv[]) {








#if 0

	Mat4x4f s = scale(Vec3f(2, 3, 4));
	Mat4x4f r = quat_to_rotation_matrix(euler_to_quat(Vec3f(90, 0, 0)));
	Mat4x4f t = translate(Vec3f(-20, 10, 2));

	Mat4x4f tr = t * r;
	Mat4x4f result = t * r * s;
	

	//Vec3f test = quat_to_euler(q);
	//
	//Mat4x4f mqt = quat_to_rotation_matrix(q);

	
	
	
	main_test();
	
#else
					


	
	
	

	Engine engine;
	
	engine.renderer.type = BackenedRenderer_OpenGL;
	
	// NOTE: Software renderer is deprecated, it doenst load the game scene
	//engine.renderer.type = BackenedRenderer_Software;
	
	
	if (init_engine(&engine)) {
		game_loop(&engine);
		destroy_engine(&engine);
	}
#endif
	
    return 0;
}


