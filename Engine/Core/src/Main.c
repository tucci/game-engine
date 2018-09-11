#pragma once

#include "Core/Engine.h"

#include "test.c"




int main(int argc, char* argv[]) {


	main_test();
	
	
	
	Engine engine;
	
	engine.renderer.type = BackenedRenderer_OpenGL;
	
	// NOTE: Software renderer is deprecated, it doenst load the game scene
	//engine.renderer.type = BackenedRenderer_Software;
	
	
	if (init_engine(&engine)) {
		game_loop(&engine);
		destroy_engine(&engine);
	}
	
    return 0;
}


