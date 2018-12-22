#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"


#include "debug_macros.h"

#define ZLIB_WINAPI



#define EDITOR_MODE 1





int main(int argc, char* argv[]) {



	
	


#if 0

	main_test();
	
#else
					
	Engine engine;
	
#if EDITOR_MODE
	// Our edior only supports opengl rendering
	engine.renderer.type = BackenedRenderer_OpenGL;
#elif
	engine.renderer.type = BackenedRenderer_OpenGL;
#endif
	
	
	// NOTE: Software renderer is deprecated, it doenst load the game scene
	//engine.renderer.type = BackenedRenderer_Software;
	

	
	if (init_engine(&engine)) {
		game_loop(&engine);
		destroy_engine(&engine);
	}
#endif
	
    return 0;
}


