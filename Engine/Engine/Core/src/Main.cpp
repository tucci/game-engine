#pragma once

#include "Core/Engine.h"

#include "test.cpp"
#include "Common/stretchy_buffer.h"

#include "Common/common_macros.h"
#include "debug_macros.h"

#include "Logger.h"


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

u32 MyThreadFunction(void* param) {

	char* str = (char*)param;
	
	
	for (int i = 0; i < 10; i++) {
		debug_print("this is a thread test %s\n", str);
	}

	p_thread_exit();
	return 0;
}


enum WindowFlag2 {
	// NOTE:(steven) These flags try to have the same values as the flags from SDL_WindowFlags
	WindowFlag2_None = 0,
	WindowFlag2_Fullscreen = 1 << 0,
	WindowFlag2_OpenGL = 1 << 1,
	WindowFlag2_Hidden = 1 << 3,
	WindowFlag2_Borderless = 1 << 4,
	WindowFlag2_Resizable = 1 << 5,
	WindowFlag2_Minimized = 1 << 6,
	WindowFlag2_Maximized = 1 << 7,
	WindowFlag2_Has_Keyboard_Focus = 1 << 9,
	WindowFlag2_Has_Mouse_Focus = 1 << 10,

};
int main(int argc, char* argv[]) {

	//typedef void(*SignalHandlerPointer)(int);
	//SignalHandlerPointer previousHandler;
	//previousHandler = signal(SIGABRT, SignalHandler);
	//previousHandler = signal(SIGTERM, SignalHandler);
	

	signal(SIGINT, handle_signals);
	signal(SIGABRT, handle_signals);
	signal(SIGTERM, handle_signals);

	g_init_logger();

#if 0


	//u32 flags = 0;
	//flags |= WindowFlag_Has_Mouse_Focus;
	//
	//bool set = (flags & WindowFlag2_Has_Mouse_Focus) != 0;
	//DEBUG_BREAK;
	//char* p1 = "t1";
	//ThreadHandle* t = p_thread_create_handle(MyThreadFunction, p1);
	//
	//
	//char* p2 = "t2";
	//ThreadHandle* t2 = p_thread_create_handle(MyThreadFunction, p2);
	//
	//p_thread_resume(t);
	//p_thread_resume(t2);
	//
	//
	//
	//p_thread_wait_for(t, INFINITE);
	//
	//p_thread_wait_for(t2, INFINITE);
	//
	//p_thread_free_handle(t);
	//p_thread_free_handle(t2);
	//
	////figure out other thread things we need? multi wait, join?
	////figure out what u dont know about threads
	//
	main_test();
	

#else
	
	
	Engine engine;
	engine.renderer.type = BackenedRendererType::OpenGL;
	
	
	
#if ENGINE_MODE_EDITOR
	// Our edior only supports opengl rendering
	engine.renderer.type = BackenedRendererType::OpenGL;
#endif
	
	
	
	// NOTE: Software renderer is deprecated, it doenst load the game scene
	//engine.renderer.type = BackenedRenderer_Software;
	
	
	
	if (init_engine(&engine)) {
#if ENGINE_MODE_EDITOR
		editor_loop(&engine);
#else
		game_loop(&engine);
#endif
		
		destroy_engine(&engine);
	}
#endif

	LOG_INFO("ENGINE", "Engine has exited successfully");
	
	g_destory_logger();
    return 0;
}


