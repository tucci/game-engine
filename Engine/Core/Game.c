#pragma once

#include "Game.h"

#include "../Common/common_macros.h"

#include "Entities/Primitives.h"



void load_scene(Game* game, int scene_id) {
	ObjModel model;

	game->loaded_scene = cast(Scene*) linear_alloc(&game->game_memory, sizeof(Scene), 4);

	Scene* scene = game->loaded_scene;


	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, cast(Vec4f){0, 0, 5, 0});



	load_obj("Assets/obj/african_head.obj", &model);
	//load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh(&model, &scene->mesh_test, &game->game_memory);
	free_obj(&model);

	make_cube(&scene->primative_test, &game->game_memory);

	



	const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	//const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";

	fill_texture_info(texture_file, &scene->texture_test);

	scene->texture_test.data = (unsigned char*) linear_alloc(
		&game->game_memory,
		scene->texture_test.width * scene->texture_test.height * scene->texture_test.channels,
		4);
	bool loaded_texture = load_and_copyto_texture(texture_file, &scene->texture_test);

}


// TODO: give pack partition in the engine
void unload_scene(Game* game, Scene* scene) {
	linear_reset(&game->game_memory);
}




