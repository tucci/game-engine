#pragma once

#include "Core/Game.h"




#include "Math/Mat.h"
#include "Math/Quaternion.h"
#include "Common/common_macros.h"

#include "Asset/AssetManager.h"
#include "Asset/FbxImport.h"


#include "Core/ECS/Component/Primitives.h"

#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"




#include "debug_macros.h"
#include "Logger.h"

void attach_engine_subsytems(Game* game, EngineAPI api) {
	game->engineAPI = api;
	
	LOG_INFO("ENGINE", "Attaching engine subsystems");
	assert(api.input != NULL);
	assert(api.display != NULL);
	assert(api.game_loop != NULL);
	assert(api.window != NULL);
	
}

void on_game_start(Game* game) {
	arena_init(&game->arena);
	size_t mem_size = GAME_MEMORY;
	void* mem_block = arena_alloc(&game->arena, mem_size);
	mem_size = game->arena.end - cast(char*) mem_block;
	stack_alloc_init(&game->stack, mem_block, mem_size);
	game->loaded_scene = cast(Scene*) stack_alloc(&game->stack, sizeof(Scene), 4);

	load_scene(game, 1);
	
}
void on_game_quit(Game* game) {
	LOG_INFO("ENGINE", "Unattaching game");

	unload_scene(game, game->loaded_scene);

	arena_free(&game->arena);
}


void load_scene(Game* game, int scene_id) {

	
	
	
	EngineAPI* api = &game->engineAPI;
	
	EntityManager* entity_manager = api->entity_manager;
	Renderer* renderer = api->renderer;
	AssetManager* asset_manager = api->asset_manager;

	Scene* scene = game->loaded_scene;
	assert(scene != NULL);
	scene->scene_id = scene_id;
	LOG_INFO("ENGINE", "Loading scene %d", scene_id);

	
	
	// Camera loading
	scene->entity_main_camera = create_entity(entity_manager, "Game Camera");
	add_component(entity_manager, scene->entity_main_camera, ComponentType::Transform);
	add_component(entity_manager, scene->entity_main_camera, ComponentType::Camera);

	Camera* cam = get_camera(entity_manager, scene->entity_main_camera);
	
	set_camera_params(entity_manager, scene->entity_main_camera, 0.0001f, 100.0f, 90.0f, api->window->size.x / cast(float) api->window->size.y);
	set_position(entity_manager, scene->entity_main_camera, Vec3f(0, 0, 0));
	scene->main_camera = get_camera(entity_manager, scene->entity_main_camera);

	


	// Mesh 1
	//scene->entity_mesh_test = create_entity(entity_manager);
	//add_component(entity_manager, scene->entity_mesh_test, ComponentType::Transform);
	//add_component(entity_manager, scene->entity_mesh_test, ComponentType::StaticMesh);

	//StaticMesh* mesh1 = get_static_mesh(entity_manager, scene->entity_mesh_test);
	//bool loaded = obj_to_static_mesh("Assets/obj/african_head.obj", mesh1, &game->stack);
	//bool loaded = obj_to_static_mesh("Assets/AC Cobra/Shelby.obj", mesh1, &game->s tack);
	//FBX_Import_Data import = import_fbx("Assets/AC Cobra/Shelby.fbx", &game->stack, true);

	//AssetImporter importer;
	//init_asset_importer(&importer, &asset_manager->asset_tracker);

	//remove_all_tracked_assets(importer.tracker);
	
	//AssetID import_scene;
	//import_scene = find_asset_by_name(importer.tracker, "mill.fbx.easset");
	

	// asset not found
	//if (import_scene.id == 0) {
	//import_scene = import_fbx(&importer, "Assets/BB8 New/test3.fbx", false);

	//import_scene = import_fbx(&importer, "Assets/cube.fbx", false);
	//import_scene = import_fbx(&importer, "Assets/plane.fbx", false);
	//import_scene = import_fbx(&importer, "Assets/sphere.fbx", false);

	

	//
	//
	//
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/mill.fbx", false);
	////import_scene = import_fbx(&importer, "Assets/AC Cobra/Shelby.FBX", false);
	//	//import_scene = import_fbx(&importer, "Assets/AC Cobra/test_bin.FBX");
	//
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/mill_test2_fz_bin.fbx", true);
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/sink_fz.fbx", false);
	//
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/cube_test.fbx");
	//
	//	//import_scene = import_fbx(&importer, "Assets/BB8 New/Sink.fbx", false);
	//	//import_scene = import_fbx(&importer, "Assets/BB8 New/Sink2.fbx", false);
	//
	//
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/car_fz2.fbx", false);
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/car_fz.fbx", false);
	//
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/diamond_upy.fbx");
	//
	//
	//	// TOO MUCH MEMORY here
	//	// TODO: need to find a way for the importer memory to scale without having a fixed memory
	//	//import_scene = import_fbx(&importer, "Assets/test_fbx/mountains.fbx", false);
	////}
	//
	//
	//
	//
	//
	//
	//
	//AssetID import_scene;
	//import_scene.id = 11;
	
	

	AssetID cube = load_asset_by_name(asset_manager, "Assets/Cube_mesh.easset");
	AssetID default_mat = load_asset_by_name(asset_manager, "Assets/Default_mat.easset");

	scene->sink = create_entity(entity_manager, "Test mesh");
	add_component(entity_manager, scene->sink, ComponentType::StaticMesh);
	add_component(entity_manager, scene->sink, ComponentType::Render);
	set_render_material(entity_manager, scene->sink, default_mat.material);
	

	set_static_mesh(entity_manager, scene->sink, cube.mesh);
	
	//scene->sink = import_asset_scene_into_scene(game, import_scene.scene);



	//destroy_asset_importer(&importer);

	Vec3f model_pos = get_position(entity_manager, scene->sink);
	set_position(entity_manager, scene->entity_main_camera, model_pos);
	
	

	scene->entity_test_light = create_entity(entity_manager, "Test Light");
	add_component(entity_manager, scene->entity_test_light, ComponentType::Light);
	

	Light light;
	light.type = LightType::DirectionalLight;
	light.dir_light.direction = Vec3f(0.00000000001f, -1.0f, 0.0f);;
	light.dir_light.color = Vec3f(1, 1, 1);;
	set_light(entity_manager, scene->entity_test_light, light);

	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "InternalAssets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->game_memory, "InternalAssets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	//load_hdr_skymap(&scene->hdr_skymap, &game->stack, "InternalAssets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");

	create_skymap(renderer, &scene->hdr_skymap);
	create_shadowmap(renderer);
	
	
	
	
	//load_texture("Assets/textures/rust_iron/rustediron2_basecolor.png", &scene->albedo_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_normal.png", &scene->normal_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_metallic.png", &scene->metallic_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_roughness.png", &scene->roughness_map, &game->game_memory, false);
	//load_texture("Assets/textures/rust_iron/rustediron2_ao.png", &scene->ao_map, &game->game_memory, false);
	//renderer->render_world.albedo_map_res = create_texture(renderer, &scene->albedo_map, true);
	//renderer->render_world.normal_map_res = create_texture(renderer, &scene->normal_map, true);
	//renderer->render_world.metallic_map_res = create_texture(renderer, &scene->metallic_map, true);
	//renderer->render_world.roughness_map_res = create_texture(renderer, &scene->roughness_map, true);
	//renderer->render_world.ao_map_res = create_texture(renderer, &scene->ao_map, true);
	// Pop textures, we already have them on the gpu
	//stack_pop(&game->stack);
	//stack_pop(&game->stack);
	//stack_pop(&game->stack);
	//stack_pop(&game->stack);
	//stack_pop(&game->stack);

		
}


void unload_scene(Game* game, Scene* scene) {
	
	LOG_INFO("ENGINE", "Unloading scene %d", scene->scene_id);
	
}


void game_update(Game* game) {

	


	
	Input* input = game->engineAPI.input;
	GameTimer* timer = game->engineAPI.game_loop;
	EntityManager* entity_manager = game->engineAPI.entity_manager;
	Scene* scene = game->loaded_scene;
	Renderer* renderer = game->engineAPI.renderer;
	float delta_time = timer->delta_time;


	Camera* camera = get_camera(entity_manager, scene->entity_main_camera);



	
	
	Vec3f new_cam_direction;
	
	if (is_key_down(input, KEYCODE_W)) { new_cam_direction += (delta_time * -forward(entity_manager, scene->entity_main_camera)); }
	if (is_key_down(input, KEYCODE_S)) { new_cam_direction += (delta_time * forward(entity_manager, scene->entity_main_camera)); }

	if (is_key_down(input, KEYCODE_D)) { new_cam_direction += (delta_time * right(entity_manager, scene->entity_main_camera)); }
	if (is_key_down(input, KEYCODE_A)) { new_cam_direction += (delta_time * -right(entity_manager, scene->entity_main_camera)); }


	if (is_key_down(input, KEYCODE_LSHIFT)) { new_cam_direction += (delta_time * up(entity_manager, scene->entity_main_camera)); }
	if (is_key_down(input, KEYCODE_LCTRL)) { new_cam_direction += (delta_time * -up(entity_manager, scene->entity_main_camera)); }

	float cam_move_scale = 10;
	Vec3f cam_pos = get_position(entity_manager, scene->entity_main_camera);
	set_position(entity_manager, scene->entity_main_camera, cam_pos + (cam_move_scale * new_cam_direction));

	//debug_print("POS [%f, %f, %f]\n", cam_pos.x, cam_pos.y, cam_pos.z);
	


	


	Vec3f new_mesh_scale = Vec3f(0, 0, 0);


	if (is_key_down(input, KEYCODE_PAGEUP)) {
		
		new_mesh_scale.x += delta_time * 1;
		new_mesh_scale.y += delta_time * 1;
		new_mesh_scale.z += delta_time * 1;
	}

	if (is_key_down(input, KEYCODE_PAGEDOWN)) {
		new_mesh_scale.x -= delta_time * 1;
		new_mesh_scale.y -= delta_time * 1;
		new_mesh_scale.z -= delta_time * 1;
	}

	Vec3f test_scale = get_scale(entity_manager, scene->sink);
	set_scale(entity_manager, scene->sink, test_scale + new_mesh_scale);








	


	

	


	Light light = get_light(entity_manager, scene->entity_test_light);


	
	
	
	if (is_key_down(input, KEYCODE_LEFT)) {
		light.dir_light.direction.z -= delta_time * 0.5f;
		
		
	}

	
	if (is_key_down(input, KEYCODE_RIGHT)) {
		light.dir_light.direction.z += delta_time * 0.5f;
	}


	Vec3f sink_dir;
	Vec3f sink_pos = get_position(entity_manager, scene->sink);

	if (is_key_down(input, KEYCODE_UP)) {
		light.dir_light.direction.x += delta_time * 0.5f;

	
		sink_dir += (delta_time * forward(entity_manager, scene->sink));
	}

	if (is_key_down(input, KEYCODE_DOWN)) {
		light.dir_light.direction.x -= delta_time * 0.5f;
		sink_dir += (delta_time * -forward(entity_manager, scene->sink));
	}

	set_position(entity_manager, scene->sink, sink_pos + sink_dir);


	//Quat old_rot = rotation(entity_manager, scene->sink);
	//Quat new_test_rot;
	//
	//
	//
	//if (input->mouse.mouse_button_left.down) {
	//	//new_test_rot *= quat_from_axis_angle(Vec3f_Up, 5.0f);
	//
	//}
	//
	//if (input->mouse.mouse_button_right.down) {
	//	//new_test_rot *= quat_from_axis_angle(Vec3f_Up, -5.0f);
	//}
	////set_rotation(entity_manager, scene->sink, new_test_rot * old_rot);

	


	set_light(entity_manager, scene->entity_test_light, light);

	if (input->mouse.mouse_button_right.down) {
		Vec2i delta_pos = input->mouse.delta_pos;

		// See world/local rotation
		// Rotate camera around world first
		Quat old_cam_rot = get_rotation(entity_manager, scene->entity_main_camera);
		Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -delta_pos.x * 0.25f) * old_cam_rot;



		


		// Create a test rotation to see if we want to rotate
		// We want to check first if this rotation causes the view to go over the clamp
		// if it does, then we wont add this rotation
		// if it doesnt, then we set the new camera rotation to the test rotation
		Quat test_new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -delta_pos.y * 0.25f);


		// TODO: fix clamping issue
		// Get the euler angle so we can see if we want to clamp the rotation
		// Vec3f euler = quat_to_euler(test_new_cam_rot);
		//// hard coded clamp between -89 and 89 degrees in radians
		//if (euler.x > -89.0f && euler.x < 89.0f) {
		new_cam_rot = test_new_cam_rot;
		//}

		set_rotation(entity_manager, scene->entity_main_camera, new_cam_rot);
	}
	

	
	


	



	

}




static void import_asset_scene_node(Game* game, EntityManager* manager, AssetImport_Scene* scene, AssetImport_SceneNode* parent_node, Entity parent_entity) {

	AssetImport_SceneNode* children = parent_node->children;
	for (u32 i = 0; i < parent_node->children_count; i++) {
		AssetImport_SceneNode* child_node = &children[i];
		Entity child_entity = create_entity(manager, child_node->name);




		//add_component(manager, child_entity, ComponentType::Transform);
		//set_name(manager, child_entity, child_node->name);
		set_position(manager, child_entity, child_node->translation);
		set_scale(manager, child_entity, child_node->scale);
		set_rotation(manager, child_entity, euler_to_quat(child_node->get_rotation));
		attach_child_entity(manager, parent_entity, child_entity);

		import_asset_scene_node(game, manager, scene, child_node, child_entity);
	}

	if (parent_node->mesh_count > 0) {
		for (u32 i = 0; i < parent_node->mesh_count; i++) {
			add_component(manager, parent_entity, ComponentType::StaticMesh);
			add_component(manager, parent_entity, ComponentType::Render);
		


			u32 mesh_index = parent_node->meshes[i];
			AssetID mesh_id = scene->mesh_infos[mesh_index];
			set_static_mesh(manager, parent_entity, mesh_id.mesh);

			// TODO: What happens when the mesh has multiple child nodes, does that mean is has multiple materials?
			//assert(parent_node->children_count < 2);
			// Get material for this mesh
			if (parent_node->children_count != 0) {
				AssetImport_SceneNode* child_node = &parent_node->children[i];
				if (child_node->material_count > 0) {
					u32 mat_index = child_node->materials[0];
					AssetID material_id = scene->material_infos[mat_index];
					set_render_material(manager, parent_entity, material_id.material);
				} else {
					// use default material
					set_render_material(manager, parent_entity, game->engineAPI.asset_manager->default_mat.material);
				}
			} else {
				// use default material
				set_render_material(manager, parent_entity, game->engineAPI.asset_manager->default_mat.material);
				
			}
			

		}
	}

}


Entity import_asset_scene_into_scene(Game* game, SceneID id) {

	AssetID scene_id;
	scene_id.id = id.id;
	scene_id.type = AssetType::Scene;
	InternalAsset asset = get_internal_asset_by_id(game->engineAPI.asset_manager, scene_id);
	
	AssetImport_Scene* scene = asset.scene;
	EntityManager* manager = game->engineAPI.entity_manager;

	
	Entity root = create_entity(manager, scene->root->name);
	//add_component(manager, root, ComponentType::Transform);
	//set_name(manager, root, scene->root->name);

	set_position(manager, root, scene->root->translation);
	set_scale(manager, root, scene->root->scale);
	set_rotation(manager, root, euler_to_quat(scene->root->get_rotation));

	
	import_asset_scene_node(game, manager, scene, scene->root, root);
	return root;
}





