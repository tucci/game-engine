#pragma once

#include "Common/LinearAllocator.h"
#include "Common/StackAllocator.h"
#include "Common/common_macros.h"


#include <stdio.h>
#include <string.h>

typedef struct TestAllocStruct {
	int x;
	int y;
	int z;
} TestAllocStruct;







void inline test_linear_alloc(void) {


	void* game_mem = malloc(1024);

	LinearAllocator la;

	linear_init(&la, game_mem, 512);

	// Test string
	char* test = cast(char*)linear_alloc(&la, 16, 16);
	if (test != NULL) {
		const char* c = "this is a test.";
		strcpy_s(test, strlen(c) + 1, c);
	}
	
	int s = 0;
	for (int i = 0; i < 40 ; i++) {
		TestAllocStruct* test2 = cast(TestAllocStruct*)linear_alloc(&la, sizeof(TestAllocStruct), 4);
		if (test2 != NULL) {
			test2->x = s + 0;
			test2->y = s + 1;
			test2->z = s + 2;
			s += 3;
			
		}
	}

	linear_reset(&la);
	

	free(game_mem);
}


void inline test_stack_alloc(void) {
	void* game_mem = malloc(1024);

	StackAllocator sa;

	stack_alloc_init(&sa, game_mem, 512);

	// Test string
	char* test = cast(char*)stack_alloc(&sa, 16, 16);
	if (test != NULL) {
		const char* c = "this is a test.";
		strcpy_s(test, strlen(c) + 1, c);
	}

	stack_pop(&sa);

	int s = 0;
	for (int i = 0; i < 10; i++) {
		TestAllocStruct* test2 = cast(TestAllocStruct*)stack_alloc(&sa, sizeof(TestAllocStruct), 4);
		if (test2 != NULL) {
			test2->x = s + 0;
			test2->y = s + 1;
			test2->z = s + 2;
			s += 3;

		}
		
	}

	stack_pop(&sa);


	free(game_mem);
}

void inline main_test(void) {
	test_linear_alloc();
	test_stack_alloc();
}
