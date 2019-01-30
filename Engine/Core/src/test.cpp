#pragma once



#include "debug_macros.h"

#include "Core/ECS/Component/Transform.h"


#include "Common/LinearAllocator.h"
#include "Common/StackAllocator.h"
#include "Common/Arena.h"
#include "Common/common_macros.h"
#include "Common/Map.h"

#include "Math/Vec.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"


#include "Core/ECS/EntityManager.h"

#include "Core/ECS/JobSystem/CameraSystem.h"



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

	char* f = cast(char*)stack_alloc(&sa, 10, 4);
	f[0] = 't';

	stack_set_reset_point(&sa);
	f = cast(char*)stack_alloc(&sa, 10, 4);
	f[0] = 'a';
	f = cast(char*)stack_alloc(&sa, 10, 4);
	f[0] = 'b';

	stack_pop_to_last_reset_point(&sa);


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



void inline test_matrix(void) {
	Mat4x4f id;
	Mat4x4f zero = Mat4x4f(0);
	Mat4x4f one = Mat4x4f(1);

}


void inline test_ecs(void) {
	EntityManager manager;
	init_entity_manager(&manager);
	Entity e1 = create_entity(&manager);
	Entity e2 = create_entity(&manager);
	Entity e3 = create_entity(&manager);
	Entity e4 = create_entity(&manager);
	Entity e5 = create_entity(&manager);

	add_component(&manager, e1, ComponentType::Camera); 
	add_component(&manager, e2, ComponentType::Camera);
	add_component(&manager, e3, ComponentType::Camera);
	
	Camera* camera = get_camera(&manager, e1);

	camera->fov = 100;
	camera->near_clip = 1;
	camera->far_clip = 200;
	camera->aspect_ratio = 2;

	


	remove_component(&manager, e1, ComponentType::Camera);
	add_component(&manager, e1, ComponentType::Camera);


	Entity test = create_entity(&manager);
	add_component(&manager, test, ComponentType::Camera);
	add_component(&manager, test, ComponentType::Transform);
	add_component(&manager, test, ComponentType::StaticMesh);
	add_component(&manager, test, ComponentType::Light);

	destroy_entity(&manager, test);
	

	assert(e1.id == 1);
	assert(e2.id == 2);
	assert(e3.id == 3);
	assert(e4.id == 4);
	assert(e5.id == 5);

	destroy_entity(&manager, e3);
	Entity e6 = create_entity(&manager);

	destroy_entity(&manager, e5);
	Entity e7 = create_entity(&manager);

	assert(e6.id == 6);

	destroy_entity(&manager, e1);
	destroy_entity(&manager, e1);

	destroy_entity(&manager, e2);
	destroy_entity(&manager, e4);
	destroy_entity(&manager, e6);
	destroy_entity(&manager, e7);

	

	Entity e8 = create_entity(&manager);
	destroy_entity_manager(&manager);
}


void inline test_arena(void) {
	Arena a;
	arena_init(&a);
	void* block = arena_alloc(&a, MEGABYTES(2));
	void* block2 = arena_alloc(&a, KILOBYTES(1));
	void* block3 = arena_alloc(&a, KILOBYTES(1));

	arena_free(&a);
		
		

}

void inline test_map(void) {
	CompactMap<u64> map;
	map_init(&map);
	map_put(&map, 1, (u64)1);
	map_put(&map, 2, (u64)2);
	map_put(&map, 3, (u64)3);
	map_put(&map, 4, (u64)4);
	map_put(&map, 5, (u64)5);
	map_put(&map, 6, (u64)6);
	map_put(&map, 7, (u64)7);
	map_put(&map, 8, (u64)8);
	map_put(&map, 9, (u64)9);
	map_put(&map, 10, (u64)10);

	map_remove(&map, 2);
	map_remove(&map, 5);
	map_remove(&map, 7);

	
	
	MapResult<u64> t1 = map_get(&map, 1);
	t1 = map_get(&map, 2);
	t1 = map_get(&map, 3);
	t1 = map_get(&map, 4);
	t1 = map_get(&map, 5);
	t1 = map_get(&map, 6);
	t1 = map_get(&map, 7);
	t1 = map_get(&map, 8);
	t1 = map_get(&map, 9);
	t1 = map_get(&map, 10);
	t1 = map_get(&map, 32);

	map_put(&map, 11, (u64)11);
	map_put(&map, 12, (u64)12);
	map_put(&map, 13, (u64)13);
	map_put(&map, 14, (u64)14);
	map_put(&map, 15, (u64)14);
	map_put(&map, 16, (u64)14);
	map_put(&map, 17, (u64)14);
	map_put(&map, 18, (u64)14);
	map_put(&map, 19, (u64)14);
	map_put(&map, 20, (u64)14);


	
	t1 = map_get(&map, 1);
	t1 = map_get(&map, 2);
	t1 = map_get(&map, 3);
	t1 = map_get(&map, 4);
	t1 = map_get(&map, 5);
	t1 = map_get(&map, 6);
	t1 = map_get(&map, 7);
	t1 = map_get(&map, 8);
	t1 = map_get(&map, 9);
	t1 = map_get(&map, 10);
	t1 = map_get(&map, 11);
	t1 = map_get(&map, 12);
	t1 = map_get(&map, 13);
	t1 = map_get(&map, 14);
	t1 = map_get(&map, 15);
	t1 = map_get(&map, 16);
	t1 = map_get(&map, 17);
	t1 = map_get(&map, 18);
	t1 = map_get(&map, 19);
	t1 = map_get(&map, 20);
	

	//map[4] = 3;
	//map[2] = 4;
	//map[5] = 5;
	//map[6] = 6;
	//map[1] = 7;
	//
	//map[3] = 7;
	//
	//t1 = map[4];
	//t2 = map[2];
	//t3 = map[5];
	//t4 = map[6];
	//t4 = map[1];
	map_destroy(&map);
}

void inline main_test(void) {
	

	//test_linear_alloc();
	test_stack_alloc();
	//test_simd_vec();
	//test_quats();
	//test_transforms();
	//test_matrix();
	//test_ecs();
	//test_arena();
	//test_map();
}
