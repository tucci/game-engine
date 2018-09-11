#pragma once

#include "Core/Game.h"




#include "Math/Mat.h"
#include "Math/Quaternion.h"
#include "Common/common_macros.h"
#include "Core/Entities/Primitives.h"

#include "debug_macros.h"

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



	





	// Init lights
	scene->test_light.direction = Vec3f(0.00000000001, -1, 0);
	

	// Camera loading
	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, Vec3f(0, 0, 0));
	scene->main_camera.aspect_ratio = api->window->size.x / cast(float) api->window->size.y;
	scene->main_camera.transform.rotation = Quat();
	
	

	


	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	
	


	//const char* texture_file = "Assets/obj/african_head_diffuse.tga";
	//const char* texture_file = "Assets/obj/diablo3_pose_diffuse.tga";
	//const char* texture_file = "Assets/obj/earth_tex.jpg";
	//const char* texture_file = "Assets/obj/test.png";


	
	
	//load_texture("Assets/textures/rust_iron/rustediron2_basecolor.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_metallic.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_ao.png", &scene->ao_map, &game->game_memory, false);

	//load_texture("Assets/textures/granite_smooth/granitesmooth1-normal2.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/granite_smooth/granitesmooth1-albedo.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/granite_smooth/granitesmooth1-metalness.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/granite_smooth/granitesmooth1-roughness3.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/granite_smooth/granitesmooth1-ao.png", &scene->ao_map, &game->game_memory, false);

	//load_texture("Assets/textures/gold-scuffed/gold-scuffed_basecolor.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/gold-scuffed/gold-scuffed_normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/gold-scuffed/gold-scuffed_metallic.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/gold-scuffed/gold-scuffed_roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/gold-scuffed/gold-scuffed_ao.png", &scene->ao_map, &game->game_memory, false);



	

	//load_texture("Assets/textures/paint_cement/wornpaintedcement-albedo.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-metalness.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-ao.png", &scene->ao_map, &game->game_memory, false);


	load_texture("Assets/textures/plastic/scuffed-plastic4-alb.png", &scene->albedo_map, &game->game_memory, false);
	load_texture("Assets/textures/plastic/scuffed-plastic-normal.png", &scene->normal_map, &game->game_memory, false);
	load_texture("Assets/textures/plastic/scuffed-plastic-metal.png", &scene->metallic_map, &game->game_memory, false);
	load_texture("Assets/textures/plastic/scuffed-plastic-rough.png", &scene->roughness_map, &game->game_memory, false);
	load_texture("Assets/textures/plastic/scuffed-plastic-ao.png", &scene->ao_map, &game->game_memory, false);

	//load_texture("Assets/textures/mahogfloor/mahogfloor_basecolor.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/mahogfloor/mahogfloor_normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/mahogfloor/mahogfloor_metal.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/mahogfloor/mahogfloor_roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/mahogfloor/mahogfloor_AO.png", &scene->ao_map, &game->game_memory, false);

	

	//load_texture("Assets/textures/bamboo-wood/bamboo-wood-semigloss-albedo.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/bamboo-wood/bamboo-wood-semigloss-normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/bamboo-wood/bamboo-wood-semigloss-metalness.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/bamboo-wood/bamboo-wood-semigloss-roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/bamboo-wood/bamboo-wood-semigloss-ao.png", &scene->ao_map, &game->game_memory, false);



	
	
	//load_obj("Assets/obj/diablo3_pose.obj", &model);
	obj_to_static_mesh("Assets/obj/african_head.obj", &scene->mesh_test, &game->game_memory);
	


	//make_cube(&scene->mesh_test, &game->game_memory);

	scene->mesh_test.transform = Transform();
	scene->mesh_test.transform.position = Vec3f(0, 0, -5);



	


	make_uv_sphere(&scene->mesh_test2, 16, 32, &game->game_memory);
	scene->mesh_test2.transform = Transform();
	scene->mesh_test2.transform.position = Vec3f(5, 0, 1);
	


	make_plane(&scene->flat_plane, &game->game_memory);
	scene->flat_plane.transform = Transform();
	scene->flat_plane.transform.position = Vec3f(0, -2, 0);
	scene->flat_plane.transform.scale = Vec3f(100, 100, 100);
	



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
	DirectionalLight* test_light = &game->loaded_scene->test_light;



	float delta_time = timer->delta_time;

	// TODO: remove need for sdl specific scan codes. convert to our own input api
	// TODO: figure out why everything is inverted
	// i think it has to do with the -1 * position, and transpose 
	if (input->keys[SDL_SCANCODE_W].down) { move_camera_in_direction(camera, -camera->transform.forward, delta_time); }
	if (input->keys[SDL_SCANCODE_S].down) { move_camera_in_direction(camera, camera->transform.forward, delta_time); }
	if (input->keys[SDL_SCANCODE_A].down) { move_camera_in_direction(camera, -camera->transform.right, delta_time); }
	if (input->keys[SDL_SCANCODE_D].down) { move_camera_in_direction(camera, camera->transform.right, delta_time); }
	if (input->keys[SDL_SCANCODE_LSHIFT].down) { move_camera_in_direction(camera, camera->transform.up, delta_time); }
	if (input->keys[SDL_SCANCODE_LCTRL].down) { move_camera_in_direction(camera, -camera->transform.up, delta_time); }






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

	if (input->mouse.mouse_button_left.down) {
		test->transform.rotation = test->transform.rotation * quat_from_axis_angle(Vec3f_Up, -5);
	}

	if (input->mouse.mouse_button_right.down) {
		test->transform.rotation = test->transform.rotation * quat_from_axis_angle(Vec3f_Right, 5);
	}



	
	if (input->keys[SDL_SCANCODE_LEFT].down) {
		test_light->direction.z -= delta_time * 0.5f;
		
	}

	if (input->keys[SDL_SCANCODE_RIGHT].down) {
		test_light->direction.z += delta_time * 0.5f;
	}


	if (input->keys[SDL_SCANCODE_UP].down) {
		test_light->direction.x += delta_time * 0.5f;
	}

	if (input->keys[SDL_SCANCODE_DOWN].down) {
		test_light->direction.x -= delta_time * 0.5f;
	}



	
	Vec2i delta_pos = input->mouse.delta_pos;
	
	
	// See world/local rotation
	// Rotate camera around world first
	camera->transform.rotation = quat_from_axis_angle(Vec3f_Up, -delta_pos.x * 0.25f) * camera->transform.rotation;


	Quat cam_rot = camera->transform.rotation * quat_from_axis_angle(Vec3f_Right, -delta_pos.y * 0.25f);
	// Get the euler angle so we can see if we want to clamp the rotation
	Vec3f euler = quat_to_euler(cam_rot);
	//debug_print("euler %f, %f, %f\n", rad_to_deg(euler.x), rad_to_deg(euler.y), rad_to_deg(euler.z);
	// hard coded clamp between -89 and 89 degrees in radians

	if (euler.x > DEG2RAD(-89) && euler.x <  DEG2RAD(89)) {
		camera->transform.rotation = cam_rot;
	}

	// then Rotate camera around local first
	//camera->transform.rotation *= quat_from_axis_angle(Vec3f_Right, -delta_pos.y * 0.25f);
	

		
	// We need this minus, because we need to local rotate, also it seems to break the lighting calcs
	Mat4x4f t = translate(-camera->transform.position);
	t = transpose(t);
	
	camera->view_mat = quat_to_rotation_matrix(camera->transform.rotation) * t;


	update_direction_vectors_for_transform(camera->transform);
	//camera->transform.forward = camera->transform.rotation * Vec3f_Forward;
	//camera->transform.up = camera->transform.rotation * Vec3f_Up;
	//camera->transform.right = cross(camera->transform.forward, camera->transform.up);
	
	
	
	
	



}



