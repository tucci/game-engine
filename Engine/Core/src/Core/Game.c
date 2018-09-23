#pragma once

#include "Core/Game.h"




#include "Math/Mat.h"
#include "Math/Quaternion.h"
#include "Common/common_macros.h"
#include "Core/ECS/Component/Primitives.h"

#include "Core/ECS/JobSystem/TransformSystem.h"

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

	


	// Camera loading
	scene->entity_main_camera = create_entity(api->entity_manager);
	add_component(api->entity_manager, scene->entity_main_camera, ComponentType_Camera);

	Camera* cam = get_camera(api->entity_manager, *scene->entity_main_camera);
	init_camera_params(cam, 0.1, 100, 90, api->window->size.x / cast(float) api->window->size.y);
	set_position(api->entity_manager, *scene->entity_main_camera, Vec3f(0, 0, 0));
	scene->main_camera = get_camera(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera);

	


	// Mesh 1
	scene->entity_mesh_test = create_entity(api->entity_manager);
	add_component(api->entity_manager, scene->entity_mesh_test, ComponentType_StaticMesh);

	StaticMesh* mesh1 = get_static_mesh(api->entity_manager, *scene->entity_mesh_test);
	obj_to_static_mesh("Assets/obj/african_head.obj", mesh1, &game->game_memory);
	set_position(api->entity_manager, *scene->entity_mesh_test, Vec3f(0, 0, -5));
	
	

	// Mesh 2
	scene->entity_mesh_test2 = create_entity(api->entity_manager);
	add_component(api->entity_manager, scene->entity_mesh_test2, ComponentType_StaticMesh);

	StaticMesh* mesh2 = get_static_mesh(api->entity_manager, *scene->entity_mesh_test2);
	make_uv_sphere(mesh2, 16, 32, &game->game_memory);
	set_position(api->entity_manager, *scene->entity_mesh_test2, Vec3f(5, 0, 1));
	


	// Mesh 3
	scene->entity_mesh_test3 = create_entity(api->entity_manager);
	add_component(api->entity_manager, scene->entity_mesh_test3, ComponentType_StaticMesh);

	StaticMesh* mesh3 = get_static_mesh(api->entity_manager, *scene->entity_mesh_test3);
	make_plane(mesh3, &game->game_memory);
	set_position(api->entity_manager, *scene->entity_mesh_test3, Vec3f(0, -2, 0));
	set_scale(api->entity_manager, *scene->entity_mesh_test3, Vec3f(100, 100, 100));
	

	attach_child_entity(api->entity_manager, *scene->entity_mesh_test, *scene->entity_mesh_test2);
	//attach_child_entity(api->entity_manager, *scene->entity_main_camera, *scene->entity_mesh_test3);
	//attach_child_entity(api->entity_manager, *scene->entity_mesh_test2, *scene->entity_mesh_test3);
	//attach_child_entity(api->entity_manager, *scene->entity_main_camera, *scene->entity_mesh_test2);
	//attach_child_entity(api->entity_manager, *scene->entity_main_camera, *scene->entity_mesh_test3);
	
	



	// Init lights
	scene->test_light.direction = Vec3f(0.00000000001, -1, 0);
	scene->test_light.color = Vec3f(1, 1, 1);

	
	

	
	
	

	

	
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	create_skymap(api->renderer, &scene->hdr_skymap);
	create_shadowmap(api->renderer);
	
	
	
	
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

	api->renderer->render_world.albedo_map_res = create_texture(api->renderer, &scene->albedo_map, true);
	api->renderer->render_world.normal_map_res = create_texture(api->renderer, &scene->normal_map, true);
	api->renderer->render_world.metallic_map_res = create_texture(api->renderer, &scene->metallic_map, true);
	api->renderer->render_world.roughness_map_res = create_texture(api->renderer, &scene->roughness_map, true);
	api->renderer->render_world.ao_map_res = create_texture(api->renderer, &scene->ao_map, true);
	// TODO: once the textures are added on the gpu, we can free them here
	
		
}


// TODO: give back partition in the input
void unload_scene(Game* game, Scene* scene) {
	linear_reset(&game->game_memory);
}


void game_update(Game* game) {

	


	
	Input* input = game->engineAPI.input;
	GameTimer* timer = game->engineAPI.game_loop;

	Camera* camera = get_camera(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera);
	
	DirectionalLight* test_light = &game->loaded_scene->test_light;


	
	float delta_time = timer->delta_time;


	

	// TODO: remove need for sdl specific scan codes. convert to our own input api
	// TODO: figure out why everything is inverted
	// i think it has to do with the -1 * position, and transpose 
	Vec3f new_cam_direction;
	
	if (input->keys[SDL_SCANCODE_W].down) { new_cam_direction = (delta_time * -forward(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }
	if (input->keys[SDL_SCANCODE_S].down) { new_cam_direction = (delta_time * forward(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }
	if (input->keys[SDL_SCANCODE_A].down) { new_cam_direction = (delta_time * -right(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }
	if (input->keys[SDL_SCANCODE_D].down) { new_cam_direction = (delta_time * right(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }
	if (input->keys[SDL_SCANCODE_LSHIFT].down) { new_cam_direction = (delta_time * up(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }
	if (input->keys[SDL_SCANCODE_LCTRL].down) { new_cam_direction = (delta_time * -up(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera)); }

	Vec3f cam_pos = position(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera);
	set_position(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera, cam_pos + new_cam_direction);


	Vec3f new_mesh_pos;

	
	if (input->keys[SDL_SCANCODE_UP].down) { new_mesh_pos = (delta_time * -forward(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2)); }
	if (input->keys[SDL_SCANCODE_DOWN].down) { new_mesh_pos = (delta_time * forward(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2)); }
	if (input->keys[SDL_SCANCODE_LEFT].down) { new_mesh_pos = (delta_time * -right(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2)); }
	if (input->keys[SDL_SCANCODE_RIGHT].down) { new_mesh_pos =  (delta_time * right(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2)); }
	

	Vec3f mesh_pos = position(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2);
	set_position(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2, mesh_pos + new_mesh_pos);


	Vec3f new_mesh_scale = Vec3f(0, 0, 0);

	if (input->keys[SDL_SCANCODE_PAGEUP].down) {
		new_mesh_scale.x += delta_time * 10;
		new_mesh_scale.y += delta_time * 10;
		new_mesh_scale.z += delta_time * 10;
	}

	if (input->keys[SDL_SCANCODE_PAGEDOWN].down) {
		new_mesh_scale.x -= delta_time * 10;
		new_mesh_scale.y -= delta_time * 10;
		new_mesh_scale.z -= delta_time * 10;
	}

	Vec3f test_scale = get_scale(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test);
	set_scale(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test, test_scale + new_mesh_scale);







	Quat new_test_rot;

	if (input->mouse.mouse_button_left.down) {
		new_test_rot *= quat_from_axis_angle(Vec3f_Up, -5);
		//test->transform.rotation = test->transform.rotation * quat_from_axis_angle(Vec3f_Up, -5);
	}

	if (input->mouse.mouse_button_right.down) {
		new_test_rot *= quat_from_axis_angle(Vec3f_Up, 5);
		//test->transform.rotation = test->transform.rotation * quat_from_axis_angle(Vec3f_Right, 5);
	}

	Quat old_test_rot = rotation(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test);
	set_rotation(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test, old_test_rot * new_test_rot);
	

	
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
	Quat old_cam_rot = rotation(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera);
	Quat new_cam_rot = new_cam_rot = quat_from_axis_angle(Vec3f_Up, -delta_pos.x * 0.25f) * old_cam_rot;
	
	

	
	

	// Create a test rotation to see if we want to rotate
	// We want to check first if this rotation causes the view to go over the clamp
	// if it does, then we wont add this rotation
	// if it doesnt, then we set the new camera rotation to the test rotation
	Quat test_new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -delta_pos.y * 0.25f);
	// Get the euler angle so we can see if we want to clamp the rotation
	Vec3f euler = quat_to_euler(test_new_cam_rot);
	// hard coded clamp between -89 and 89 degrees in radians
	if (euler.x > DEG2RAD(-89) && euler.x <  DEG2RAD(89)) {
		new_cam_rot = test_new_cam_rot;
	}

	set_rotation(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera, new_cam_rot);
	
	

	cam_pos = position(game->engineAPI.entity_manager, *game->loaded_scene->entity_main_camera);
	// We need this minus, because we need to local rotate, also it seems to break the lighting calcs
	Mat4x4f t = translate(-cam_pos);
	t = transpose(t);
	
	camera->view_mat = quat_to_rotation_matrix(new_cam_rot) * t;


	job_update_basis_vectors(&game->engineAPI.entity_manager->comp_manager.transform_manager);
	job_compute_world_matrices(&game->engineAPI.entity_manager->comp_manager.transform_manager);
		
	
	
	
	RenderMesh desc1;
	desc1.material_id = 0;
	desc1.mesh = get_static_mesh(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test);
	desc1.world= get_world_mat(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test);
	push_render_object(game->engineAPI.renderer, desc1);

	RenderMesh desc2;
	desc2.material_id = 0;
	desc2.mesh = get_static_mesh(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2);
	desc2.world = get_world_mat(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test2);
	push_render_object(game->engineAPI.renderer, desc2);

	RenderMesh desc3;
	desc3.material_id = 0;
	desc3.mesh = get_static_mesh(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test3);
	desc3.world = get_world_mat(game->engineAPI.entity_manager, *game->loaded_scene->entity_mesh_test3);
	push_render_object(game->engineAPI.renderer, desc3);


	push_camera(game->engineAPI.renderer, camera, cam_pos);
	push_light(game->engineAPI.renderer, test_light);
	


	
}



