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



	





	// Init lights
	scene->test_light.direction = make_vec3f(1, 1, 0);
	scene->test_light2.direction = make_vec3f(0, 1, 1);
	

	// Camera loading
	init_camera_default(&scene->main_camera);
	set_camera_pos(&scene->main_camera, make_vec3f(0, 0, 0));
	scene->main_camera.aspect_ratio = api->window->size.x / cast(float) api->window->size.y;
	scene->main_camera.transform.euler_angles = Vec3f_Zero;

	



	load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	
	


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

	load_texture("Assets/textures/gold-scuffed/gold-scuffed_basecolor.png", &scene->albedo_map, &game->game_memory, false);
	load_texture("Assets/textures/gold-scuffed/gold-scuffed_normal.png", &scene->normal_map, &game->game_memory, false);
	load_texture("Assets/textures/gold-scuffed/gold-scuffed_metallic.png", &scene->metallic_map, &game->game_memory, false);
	load_texture("Assets/textures/gold-scuffed/gold-scuffed_roughness.png", &scene->roughness_map, &game->game_memory, false);
	load_texture("Assets/textures/gold-scuffed/gold-scuffed_ao.png", &scene->ao_map, &game->game_memory, false);



	

	//load_texture("Assets/textures/paint_cement/wornpaintedcement-albedo.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-metalness.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/paint_cement/wornpaintedcement-ao.png", &scene->ao_map, &game->game_memory, false);


	//load_texture("Assets/textures/plastic/scuffed-plastic4-alb.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/plastic/scuffed-plastic-normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/plastic/scuffed-plastic-metal.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/plastic/scuffed-plastic-rough.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/plastic/scuffed-plastic-ao.png", &scene->ao_map, &game->game_memory, false);

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

	init_transform(&scene->mesh_test.transform);
	scene->mesh_test.transform.position = make_vec3f(0, 0, -5);



	


	make_uv_sphere(&scene->mesh_test2, 16, 32, &game->game_memory);
	init_transform(&scene->mesh_test2.transform);
	scene->mesh_test2.transform.position = make_vec3f(5, 0, 1);
	


	make_plane(&scene->flat_plane, &game->game_memory);
	init_transform(&scene->flat_plane.transform);
	scene->flat_plane.transform.position = make_vec3f(0, -5, 0);
	scene->flat_plane.transform.scale = make_vec3f(100, 100, 100);
	



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
	if (input->keys[SDL_SCANCODE_S].down) { move_camera_in_direction(camera, camera->transform.forward, delta_time); }
	if (input->keys[SDL_SCANCODE_W].down) { move_camera_in_direction(camera, v3_negate(camera->transform.forward), delta_time); }
	if (input->keys[SDL_SCANCODE_D].down) { move_camera_in_direction(camera, v3_negate(camera->transform.right), delta_time); }
	if (input->keys[SDL_SCANCODE_A].down) { move_camera_in_direction(camera, camera->transform.right, delta_time); }
	if (input->keys[SDL_SCANCODE_LSHIFT].down) { move_camera_in_direction(camera, camera->transform.up, delta_time); }
	if (input->keys[SDL_SCANCODE_LCTRL].down) { move_camera_in_direction(camera, v3_negate(camera->transform.up), delta_time); }




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
		test->transform.euler_angles.y -= 1.0f;
	}

	if (input->mouse.mouse_button_right.down) {
		test->transform.euler_angles.y += 1.0f;
	}



	
	if (input->keys[SDL_SCANCODE_LEFT].down) {
		test_light->direction.z -= delta_time * 10.0f;
		
	}

	if (input->keys[SDL_SCANCODE_RIGHT].down) {
		test_light->direction.z += delta_time * 10.0f;
	}


	if (input->keys[SDL_SCANCODE_UP].down) {
		test_light->direction.x += delta_time * 10.0f;;
	}

	if (input->keys[SDL_SCANCODE_DOWN].down) {
		test_light->direction.x -= delta_time * 10.0f;
	}


	

	
	
		

	
	Vec2i delta_pos = input->mouse.delta_pos;


	camera->transform.euler_angles.y += (-delta_pos.x * 0.25f);
	camera->transform.euler_angles.x += (-delta_pos.y * 0.25f);

	camera->transform.euler_angles.x = clamp(camera->transform.euler_angles.x, -89, 89);
	camera->transform.euler_angles.y = fmod(camera->transform.euler_angles.y, 360.0f);
		

	Mat4x4f t = translate(v3_multiply(-1, camera->transform.position));
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


}



