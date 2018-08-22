#pragma once

#include "Game.h"


#include "../Math/Mat.h"

#include "../Common/common_macros.h"

#include "Entities/Primitives.h"



void load_scene(Game* game, int scene_id) {
	ObjModel model;

	game->loaded_scene = cast(Scene*) linear_alloc(&game->game_memory, sizeof(Scene), 4);

	Scene* scene = game->loaded_scene;

	load_skybox(&scene->skybox,
		&game->game_memory,
		"Assets/skyboxes/stonegods/sgod_lf.tga",
		"Assets/skyboxes/stonegods/sgod_rt.tga",
		"Assets/skyboxes/stonegods/sgod_bk.tga",
		"Assets/skyboxes/stonegods/sgod_ft.tga",
		"Assets/skyboxes/stonegods/sgod_up.tga",
		"Assets/skyboxes/stonegods/sgod_dn.tga");

	
	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, cast(Vec3f){0, 0, 5});

	// TODO: this should be taken from the window
	scene->main_camera.aspect_ratio = 1280.0f / 1024.0f;




	load_obj("Assets/obj/african_head.obj", &model);
	//load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh(&model, &scene->mesh_test, &game->game_memory);
	free_obj(&model);

	make_cube(&scene->primative_test, &game->game_memory);

	



	const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	//const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";

	// TODO: instead of having a pre fill check, we can probably pass the alloactor to the load texture function
	fill_texture_info(texture_file, &scene->texture_test);
	scene->texture_test.data = (unsigned char*) linear_alloc(
		&game->game_memory,
		scene->texture_test.width * scene->texture_test.height * scene->texture_test.channels,
		4);
	bool loaded_texture = load_and_copyto_texture(texture_file, &scene->texture_test, true);

}


// TODO: give back partition in the input
void unload_scene(Game* game, Scene* scene) {
	linear_reset(&game->game_memory);
}


void game_update(Game* game, Input* input, GameTimer* timer) {


	Camera* camera = &game->loaded_scene->main_camera;
		

	if (input->mouse.mouse_button_left.down) {
		Vec2i delta_pos = input->mouse.delta_pos;

		camera->right = v3_cross(camera->forward, camera->up);

	/*	camera->orientation.x += -delta_pos.x * 0.0025f;
		camera->orientation.y += -delta_pos.y * 0.0025f;

		
		camera->orientation.y = clamp(camera->orientation.y, -89.0f, 89.0f);

		
		camera->forward.x = cosf(camera->orientation.x) * cosf(camera->orientation.y);
		camera->forward.y = sinf(camera->orientation.y);
		camera->forward.z = sinf(camera->orientation.x) * cosf(camera->orientation.y);
		camera->forward = v3_normalize(camera->forward);*/
		

		
		Vec3f origin_offset = v3_sub(Vec3f_Zero, camera->pos);

		
		Mat4x4f m1 = rotate(-delta_pos.x * 0.0025f, camera->up);
		Mat4x4f m2 = rotate(-delta_pos.y * 0.0025f, camera->right);

		Mat4x4f rot_xy = mat4x4_mul(&m1, &m2);
		

		
		
		Vec4f new_dir = make_vec4f(camera->forward.x, camera->forward.y, camera->forward.z, 1);
		new_dir = v4_normalize(new_dir);
		
		camera->forward =  mat4x4_vec_mul(&rot_xy, new_dir).xyz;

		
		
	}



	

	
	

	float delta_time = timer->delta_time;
	// TODO: remove need for sdl specific scan codes. convert to our own input api
	if (input->keys[SDL_SCANCODE_W].down)      {move_camera_in_direction(camera, Vec3f_Backward, delta_time);}
	if (input->keys[SDL_SCANCODE_S].down)      {move_camera_in_direction(camera, Vec3f_Forward, delta_time);}
	if (input->keys[SDL_SCANCODE_A].down)      {move_camera_in_direction(camera, Vec3f_Left, delta_time);}
	if (input->keys[SDL_SCANCODE_D].down)      {move_camera_in_direction(camera, Vec3f_Right, delta_time);}
	if (input->keys[SDL_SCANCODE_LSHIFT].down) {move_camera_in_direction(camera, Vec3f_Up, delta_time);}
	if (input->keys[SDL_SCANCODE_LCTRL].down)  {move_camera_in_direction(camera, Vec3f_Down, delta_time);}

}



