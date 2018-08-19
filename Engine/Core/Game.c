#pragma once

#include "Game.h"

#include "../Common/common_macros.h"



void load_scene(struct Game* game, int scene_id) {
	ObjModel model;

	game->loaded_scene = cast(Scene*) linear_alloc(&game->game_memory, sizeof(Scene), 4);

	Scene* scene = game->loaded_scene;


	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, Vec4f_Zero);



	load_obj("Assets/obj/african_head.obj", &model);
	////load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh(&model, &scene->mesh_test);
	free_obj(&model);



	const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	////const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";

	fill_texture_info(texture_file, &scene->texture_test);

	scene->texture_test.data = (unsigned char*)linear_alloc(
		&game->game_memory,
		scene->texture_test.width * scene->texture_test.height * scene->texture_test.channels,
		4);
	bool loaded_texture = load_and_copyto_texture(texture_file, &scene->texture_test);

}


void unload_scene(Game* game, Scene* scene) {

	free_static_mesh(&scene->mesh_test);
}




