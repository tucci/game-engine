#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"


#include "debug_macros.h"

#define ZLIB_WINAPI





#include "engine_platform.h"

int main(int argc, char* argv[]) {



	
	


#if 0
	
	
	char buffer[260];

	//FileHandle h = platform_create_file("test.txt");
	//
	//if (h.win_handle) {
	//	platform_path(h, buffer, 256);
	//	s64 fsize = platform_get_file_size(h);
	//	platform_close_file(h);
	//}
	

	

	IString path = IString("Assets\\test_fbx");
	IString file = IString("cube_test.fbx");
	bool x = platform_concat_path_and_filename(path, file, buffer, 260);
	
	//IString p = IString(platform_file_dirname(s.buf));
	//bool result = platform_file_dirname(s.buf, buffer, 260);

	
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


