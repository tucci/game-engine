#pragma once

#include "Game.h"

#include "../Math/Mat.h"
#include "../Math/Quaternion.h"

#include "../Common/common_macros.h"
#include "../debug_macros.h"

#include "Entities/Primitives.h"

void attach_engine_subsytems(Game* game, EngineAPI api) {
	game->engineAPI = api;

	assert(api.input != NULL);
	assert(api.display != NULL);
	assert(api.game_loop != NULL);
	assert(api.window != NULL);
}


void load_scene(Game* game, int scene_id) {

	
	game->loaded_scene = cast(Scene*) linear_alloc(&game->game_memory, sizeof(Scene), 4);


	Scene* scene = game->loaded_scene;

	//// Init root node
	//scene->root.type = SceneNodeType_Root;
	//scene->root.parent = NULL;
	//scene->root.children = NULL;
	//scene->root.child_count = 0;
	

	
	assert(scene != NULL);
	
	
	EngineAPI* api = &game->engineAPI;



	







	

	// Camera loading
	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, cast(Vec3f){0, 0, 5});
	scene->main_camera.aspect_ratio = api->window->size.x / cast(float) api->window->size.y;
	scene->main_camera.transform.euler_angles = Vec3f_Zero;

	

	
	load_skybox(&scene->skybox,
		&game->game_memory,
		"Assets/skyboxes/stonegods/sgod_lf.tga",
		"Assets/skyboxes/stonegods/sgod_rt.tga",
		"Assets/skyboxes/stonegods/sgod_bk.tga",
		"Assets/skyboxes/stonegods/sgod_ft.tga",
		"Assets/skyboxes/stonegods/sgod_up.tga",
		"Assets/skyboxes/stonegods/sgod_dn.tga");

	ObjModel model;
	//load_obj("Assets/obj/african_head.obj", &model);
	load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh(&model, &scene->mesh_test, &game->game_memory);
	free_obj(&model);


	init_transform(&scene->mesh_test.transform);

	//scene->mesh_test.transform.scale = make_vec3f(1, 1, 1);
	scene->mesh_test.transform.position = make_vec3f(5, 0, 0);
	scene->mesh_test.transform.euler_angles.y = 90.0f;


	init_transform(&scene->primative_test.transform);
	//make_cube(&scene->primative_test, &game->game_memory);
	make_uv_sphere(&scene->primative_test, 16, 32, &game->game_memory);

	



	//const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";
	//const char* texture_file = "Assets/obj/earth_tex.jpg";
	//const char* texture_file = "Assets/obj/test.png";

	
	bool loaded_texture = load_texture(texture_file, &scene->texture_test, &game->game_memory, false);

}


// TODO: give back partition in the input
void unload_scene(Game* game, Scene* scene) {
	linear_reset(&game->game_memory);
}


void game_update(Game* game) {


	Camera* camera = &game->loaded_scene->main_camera;
	Input* input = game->engineAPI.input;
	GameTimer* timer = game->engineAPI.game_loop;

	StaticMesh* test = &game->loaded_scene->mesh_test;



	float delta_time = timer->delta_time;

	// TODO: remove need for sdl specific scan codes. convert to our own input api
	if (input->keys[SDL_SCANCODE_W].down) { move_camera_in_direction(camera, camera->transform.forward, delta_time); }
	if (input->keys[SDL_SCANCODE_S].down) { move_camera_in_direction(camera, v3_negate(camera->transform.forward), delta_time); }
	if (input->keys[SDL_SCANCODE_A].down) { move_camera_in_direction(camera, v3_negate(camera->transform.right), delta_time); }
	if (input->keys[SDL_SCANCODE_D].down) { move_camera_in_direction(camera, camera->transform.right, delta_time); }
	if (input->keys[SDL_SCANCODE_LSHIFT].down) { move_camera_in_direction(camera, v3_negate(camera->transform.up), delta_time); }
	if (input->keys[SDL_SCANCODE_LCTRL].down) { move_camera_in_direction(camera, camera->transform.up, delta_time); }



	// Test inputs on model
	if (input->keys[SDL_SCANCODE_LEFT].down) {
		test->transform.position.z -= delta_time * 10.0f;
		
	}

	if (input->keys[SDL_SCANCODE_PAGEUP].down) {
		test->transform.scale.x += delta_time * 10;
		test->transform.scale.y += delta_time * 10;
		test->transform.scale.z += delta_time * 10;
	}

	if (input->keys[SDL_SCANCODE_PAGEDOWN].down) {
		test->transform.scale.x -= delta_time * 10;
		test->transform.scale.y -= delta_time * 10;
		test->transform.scale.z -= delta_time * 10;
	}


	if (input->keys[SDL_SCANCODE_RIGHT].down) {
		test->transform.position.z += delta_time * 10.0f;
		
	}


	if (input->keys[SDL_SCANCODE_UP].down) {
		test->transform.position.x += delta_time * 10.0f;;
	}

	if (input->keys[SDL_SCANCODE_DOWN].down) {
		test->transform.position.x -= delta_time * 10.0f;
	}


	if (input->mouse.mouse_button_left.down) {
		test->transform.euler_angles.y -= 1.0f;
	}

	if (input->mouse.mouse_button_right.down) {
		test->transform.euler_angles.y += 1.0f;
	}

	
	
		

	
	Vec2i delta_pos = input->mouse.delta_pos;


	camera->transform.euler_angles.y += (-delta_pos.x * 0.25f);
	camera->transform.euler_angles.x += (-delta_pos.y * 0.25f);

	camera->transform.euler_angles.x = clamp(camera->transform.euler_angles.x, -89, 89);
	camera->transform.euler_angles.y = fmod(camera->transform.euler_angles.y, 360.0f);
		

	Mat4x4f t = translate(v3_multiply(1, camera->transform.position));
	t = transpose(&t);
	

	Quat q = quat_from_axis_angle(Vec3f_Up, camera->transform.euler_angles.y);
	q = quat_mult(q, quat_from_axis_angle(Vec3f_Right, camera->transform.euler_angles.x));


	Mat4x4f rot_xy = quat_to_rotation_matrix(q);
	Mat4x4f posrot = mat4x4_mul(&rot_xy, &t);

	
	// Convert euler angles (yaw,pitch) to our forward vector
	camera->transform.forward = euler_to_vector(camera->transform.euler_angles);
	camera->transform.forward.y *= -1; // flip y axis
	
	
				
	camera->transform.right = v3_cross(camera->transform.forward, camera->transform.up);
	camera->view_mat = posrot;

	// TODO: we also need to update the up vector since it could change when we change the x angle >

	//debug_print("orientation %f, %f, %f, forward, %f, %f, %f \n",
	//	camera->transform.euler_angles.x, camera->transform.euler_angles.y, camera->transform.euler_angles.z,
	//	camera->transform.forward.x, camera->transform.forward.y, camera->transform.forward.z);

}



