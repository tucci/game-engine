#pragma once

#include "Common/LinearAllocator.h"
#include "Common/StackAllocator.h"
#include "Common/common_macros.h"
#include "Math/Vec.h"

#include "debug_macros.h"
#include "Math/Math.h"



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


#define assert_vec(v1, v2, eps) assert(F_EQUAL_ABS(v1.x, v2.x, eps) == 1 && F_EQUAL_ABS(v1.y, v2.y, eps) == 1 && F_EQUAL_ABS(v1.z, v2.z, eps) == 1 && F_EQUAL_ABS(v1.w, v2.w, eps) == 1)

void inline test_simd_vec(void) {
	Vec4f v1 = ToVec4f(1, 2, 3, 4);
	Vec4f v2 = ToVec4f(5, 6, 7, 8);

	Vec4f result = v4_add(v1, v2);
	//assert_vec(v1, ToVec4f(6.0f, 8.0f, 10.0f, 12.0f), 0.1f);
	result = v4_negate(v1);
	result = v4_sub(v1, v2);
	result = v4_multiply(5, v2);
	float dot = v4_dot(v1, v2);
	float length = v4_length(v1);
	result = v4_normalize(v1);
	result.xyz = v3_cross(v1.xyz, v2.xyz);
}

void inline main_test(void) {
	test_linear_alloc();
	test_stack_alloc();
	test_simd_vec();
}
