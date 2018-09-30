#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"



int main(int argc, char* argv[]) {


#if 0

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


