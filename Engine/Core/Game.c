#pragma once

#include "Game.h"

#include "../Math/Mat.h"

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

	
	assert(scene != NULL);
	

	EngineAPI* api = &game->engineAPI;

	

	// Camera loading
	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, cast(Vec3f){0, 0, 5});
	scene->main_camera.aspect_ratio = api->window->size.x / cast(float) api->window->size.y;
	scene->main_camera.orientation = Vec3f_Zero;

	

	
	load_skybox(&scene->skybox,
		&game->game_memory,
		"Assets/skyboxes/stonegods/sgod_lf.tga",
		"Assets/skyboxes/stonegods/sgod_rt.tga",
		"Assets/skyboxes/stonegods/sgod_bk.tga",
		"Assets/skyboxes/stonegods/sgod_ft.tga",
		"Assets/skyboxes/stonegods/sgod_up.tga",
		"Assets/skyboxes/stonegods/sgod_dn.tga");

	ObjModel model;
	load_obj("Assets/obj/african_head.obj", &model);
	//load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh(&model, &scene->mesh_test, &game->game_memory);
	free_obj(&model);

	make_cube(&scene->primative_test, &game->game_memory);

	



	const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	//const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";

	
	bool loaded_texture = load_texture(texture_file, &scene->texture_test, &game->game_memory, true);

}


// TODO: give back partition in the input
void unload_scene(Game* game, Scene* scene) {
	linear_reset(&game->game_memory);
}


void game_update(Game* game) {


	Camera* camera = &game->loaded_scene->main_camera;
	Input* input = game->engineAPI.input;
	GameTimer* timer = game->engineAPI.game_loop;




	float delta_time = timer->delta_time;
	// TODO: remove need for sdl specific scan codes. convert to our own input api
	if (input->keys[SDL_SCANCODE_S].down) { move_camera_in_direction(camera, v3_negate(camera->forward), delta_time); }
	if (input->keys[SDL_SCANCODE_W].down) { move_camera_in_direction(camera, camera->forward, delta_time); }
	if (input->keys[SDL_SCANCODE_D].down) { move_camera_in_direction(camera, camera->right, delta_time); }
	if (input->keys[SDL_SCANCODE_A].down) { move_camera_in_direction(camera, v3_negate(camera->right), delta_time); }
	if (input->keys[SDL_SCANCODE_LSHIFT].down) { move_camera_in_direction(camera, v3_negate(camera->up), delta_time); }
	if (input->keys[SDL_SCANCODE_LCTRL].down) { move_camera_in_direction(camera, camera->up, delta_time); }
		

	
	Vec2i delta_pos = input->mouse.delta_pos;


	camera->orientation.y += (-delta_pos.x * 0.25f);
	camera->orientation.x += (-delta_pos.y * 0.25f);
	camera->orientation.x = clamp(camera->orientation.x, -89, 89);
	camera->orientation.y = fmod(camera->orientation.y, 360.0f);
		

	Mat4x4f t = translate(v3_multiply(1, camera->pos));
	t = transpose(&t);
	

	//Mat4x4f m1 = rotate(-delta_pos.x * 0.0025f, camera->up);
	//Mat4x4f m2 = rotate(-delta_pos.y * 0.0025f, camera->right);

	Mat4x4f m1 = rotate(deg_to_rad(camera->orientation.y), Vec3f_Up);
	Mat4x4f m2 = rotate(deg_to_rad(camera->orientation.x), Vec3f_Right);

	Mat4x4f rot_xy = mat4x4_mul(&m2, &m1);
	Mat4x4f posrot = mat4x4_mul(&rot_xy, &t);

	// TODO: move to quats
	
	// Convert oriention (yaw,pitch) to our forward vector
	camera->forward = euler_to_vector(camera->orientation);
	camera->forward.y *= -1; // flip y axis
		


		
	camera->right = v3_cross(camera->forward, camera->up);
	camera->view_mat = posrot;
	debug_print("orientation %f, %f, %f, forward, %f, %f, %f \n",
		camera->orientation.x, camera->orientation.y, camera->orientation.z,
		camera->forward.x, camera->forward.y, camera->forward.z);

	////Vec4f forward4 = vec3f_to_vec4f(camera->forward, 0);
	////forward4 = v4_normalize(forward4);
	////camera->forward = mat4x4_vec_mul(&posrot, forward4).xyz;
	//camera->view_mat = look_at(camera->pos, v3_add(camera->pos, camera->forward), Vec3f_Up);
	


}



