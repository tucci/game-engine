#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"

#include "Common/common_macros.h"
#include "debug_macros.h"

#define ZLIB_WINAPI










int main(int argc, char* argv[]) {


	

	



	
	

#if 0

	main_test();
	
#else
					
	Engine engine;
	// Our edior only supports opengl rendering
	engine.renderer.type = BackenedRenderer_OpenGL;
	
	
#if ENGINE_MODE_EDITOR
	//if (argc > 1) {
	//	int hwnd_int = atoi(argv[1]);
	//	engine.editor.editor_hwnd = (HWND)hwnd_int;
	//} else {
	//	assert_fail();
	//}
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


