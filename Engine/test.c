#pragma once



#include "debug_macros.h"

#include "Core/Transform.h"


#include "Common/LinearAllocator.h"
#include "Common/StackAllocator.h"
#include "Common/common_macros.h"

#include "Math/Vec.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"





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
	Vec4f v1 = Vec4f(1, 2, 3, 4);
	Vec4f v2 = Vec4f(5, 6, 7, 8);

	Vec4f result = v1 + v2;
	//assert_vec(v1, ToVec4f(6.0f, 8.0f, 10.0f, 12.0f), 0.1f);
	result = -v1;
	result = v1 - v2;
	result = 5 * v2;
	float dotproduct = dot(v1, v2);
	float length = magnitude(v1);
	result = normalize(v1);
	result.xyz = cross(v1.xyz, v2.xyz);
}


void inline test_quats(void) {
	Quat a;
	a = quat_from_axis_angle(Vec3f(1, 0, 0), 0);
	a = quat_from_axis_angle(Vec3f(1, 0, 0), 90);
	a = quat_from_axis_angle(Vec3f(1, 0, 0), 180);

	a = quat_from_axis_angle(Vec3f(0, 1, 0), 0);
	a = quat_from_axis_angle(Vec3f(0, 1, 0), 90);
	a = quat_from_axis_angle(Vec3f(0, 1, 0), 180);



	a = quat_from_axis_angle(Vec3f(0, 0, 1), 0);
	a = quat_from_axis_angle(Vec3f(0, 0, 1), 90);
	a = quat_from_axis_angle(Vec3f(0, 0, 1), 180);


	a = quat_from_axis_angle(Vec3f(0, 1, 0), 270);

	Vec3f pt = Vec3f(1, 0, 0);
	Vec3f result = a * pt;

}

void inline test_transforms(void) {
	Transform t;
	//init_transform(&t);


	Mat4x4f m = trs_mat_from_transform(&t);


	//t.scale = Vec3f(5, 2, 4);
	t.position = Vec3f(10, 2, 3);

	//t.euler_angles.x = 90.0f;
	//t.euler_angles.y = 45.0f;
	//t.euler_angles.z = 180.0f;
	//m = trs_mat_from_transform(&t);



}


void inline test_matrix(void) {
	Mat4x4f id;
	Mat4x4f zero = Mat4x4f(0);
	Mat4x4f one = Mat4x4f(1);

}

void inline main_test(void) {
	

	test_linear_alloc();
	test_stack_alloc();
	test_simd_vec();
	test_quats();
	test_transforms();
	test_matrix();
}
