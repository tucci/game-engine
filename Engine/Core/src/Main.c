#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"


#include "debug_macros.h"

#define ZLIB_WINAPI






int main(int argc, char* argv[]) {








#if 0
	
	

	
	Texture2D td;
	td.data = NULL;
	td.uv_translation.x = 99999999999;
	td.uv_translation.y = 99999999999;
	td.uv_scaling.x = 99999999999;
	td.uv_scaling.y = 99999999999;
	td.uv_rotation = 99999999999;
	td.width = 99999999999;
	td.height = 99999999999;
	td.channels = 99999999999;
	td.depth = 99999999999;
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


