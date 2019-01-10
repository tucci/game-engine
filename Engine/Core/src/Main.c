#pragma once

#include "Core/Engine.h"

#include "test.c"
#include "Common/stretchy_buffer.h"

#include "Common/common_macros.h"
#include "debug_macros.h"


#define ZLIB_WINAPI




#include<stdio.h>
#include<signal.h>  // for handling signal 

void handle_signals(int sig) {
	switch (sig) {
		case SIGINT: {
			printf("Caught sigint %d\n", sig);
			break;
		}
		case SIGTERM: {
			printf("Caught sigterm %d\n", sig);
			break;
		}
		case SIGABRT: {
			printf("Caught sigabrt %d\n", sig);
			break;
		}
	}
	
}



void SignalHandler(int signal) {
	if (signal == SIGABRT) {
		// abort signal handler code
		printf("Caught SIGABRT %d\n");
	} else if (signal == SIGTERM){
		// ...
		printf("Caught SIGTERM %d\n");
	}

	else if (signal == SIGINT) {
		// ...
		printf("Caught SIGINT %d\n");
	}
	
}




int main(int argc, char* argv[]) {

	//typedef void(*SignalHandlerPointer)(int);
	//SignalHandlerPointer previousHandler;
	//previousHandler = signal(SIGABRT, SignalHandler);
	//previousHandler = signal(SIGTERM, SignalHandler);
	

	signal(SIGINT, handle_signals);
	signal(SIGABRT, handle_signals);
	signal(SIGTERM, handle_signals);

	

#if 0
	

	//main_test();
	

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


