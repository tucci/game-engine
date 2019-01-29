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


u32 MyThreadFunction(void* param) {

	char* str = (char*)param;
	
	
	for (int i = 0; i < 10; i++) {
		debug_print("this is a thread test %s\n", str);
	}

	p_thread_exit();
	return 0;
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
	
	
	
	char* p1 = "t1";
	ThreadHandle* t = p_thread_create_handle(MyThreadFunction, p1);
	
	
	char* p2 = "t2";
	ThreadHandle* t2 = p_thread_create_handle(MyThreadFunction, p2);
	
	p_thread_resume(t);
	p_thread_resume(t2);
	
	
	
	p_thread_wait_for(t, INFINITE);
	
	p_thread_wait_for(t2, INFINITE);
	
	p_thread_free_handle(t);
	p_thread_free_handle(t2);
	
	//figure out other thread things we need? multi wait, join?
	//figure out what u dont know about threads
	
	//main_test();
	

#else
	
	
	Engine engine;
	engine.renderer.type = BackenedRenderer_OpenGL;
	
	
	
#if ENGINE_MODE_EDITOR
	// Our edior only supports opengl rendering
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


