#pragma once

#include "SDLApp.h"
#include "test.c"







int main(int argc, char* argv[]) {

	//main_test();

	
	
	
	
	

	Engine engine;

	engine.renderer.type = BackenedRenderer_OpenGL;
	//engine.renderer.type = BackenedRenderer_Software;


	if (init_engine(&engine)) {
		game_loop(&engine);
		destroy_engine(&engine);
	}

    return 0;
}


