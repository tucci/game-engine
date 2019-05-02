#pragma once

#include "Editor/EditorInterface.h"
#include "Logger.h"


#include "SDL_syswm.h"




static void import_scene_node(EditorInterface* editor, AssetImport_Scene* scene, AssetImport_SceneNode* parent_node, Entity parent_entity) {

	EntityManager* entity_manager = editor->api.entity_manager;
	AssetManager* asset_manager = editor->api.asset_manager;
	AssetImport_SceneNode* children = parent_node->children;
	for (u32 i = 0; i < parent_node->children_count; i++) {
		AssetImport_SceneNode* child_node = &children[i];
		Entity child_entity = create_entity(entity_manager, child_node->name);




		//add_component(manager, child_entity, ComponentType::Transform);
		//set_name(manager, child_entity, child_node->name);
		set_position(entity_manager, child_entity, child_node->translation);
		set_scale(entity_manager, child_entity, child_node->scale);
		set_rotation(entity_manager, child_entity, euler_to_quat(child_node->get_rotation));
		attach_child_entity(entity_manager, parent_entity, child_entity);

		import_scene_node(editor, scene, child_node, child_entity);
	}

	if (parent_node->mesh_count > 0) {
		for (u32 i = 0; i < parent_node->mesh_count; i++) {
			add_component(entity_manager, parent_entity, ComponentType::StaticMesh);
			add_component(entity_manager, parent_entity, ComponentType::Render);



			u32 mesh_index = parent_node->meshes[i];
			AssetID mesh_id = scene->mesh_infos[mesh_index];
			set_static_mesh(entity_manager, parent_entity, mesh_id.mesh);

			// TODO: What happens when the mesh has multiple child nodes, does that mean is has multiple materials?
			//assert(parent_node->children_count < 2);
			// Get material for this mesh
			if (parent_node->children_count != 0) {
				AssetImport_SceneNode* child_node = &parent_node->children[i];
				if (child_node->material_count > 0) {
					u32 mat_index = child_node->materials[0];
					AssetID material_id = scene->material_infos[mat_index];
					set_render_material(entity_manager, parent_entity, material_id.material);
				} else {
					// use default material
					set_render_material(entity_manager, parent_entity, asset_manager->default_mat.material);
				}
			} else {
				// use default material
				set_render_material(entity_manager, parent_entity, asset_manager->default_mat.material);

			}


		}
	}

}

Entity import_scene(EditorInterface* editor, SceneID id) {

	AssetID scene_id;
	scene_id.id = id.id;
	scene_id.type = AssetType::Scene;
	
	InternalAsset asset = get_asset_by_id(editor->api.asset_manager, scene_id);

	AssetImport_Scene* scene = asset.scene;
	EntityManager* manager = editor->api.entity_manager;


	Entity root = create_entity(manager, scene->root->name);
	//add_component(manager, root, ComponentType::Transform);
	//set_name(manager, root, scene->root->name);

	set_position(manager, root, scene->root->translation);
	set_scale(manager, root, scene->root->scale);
	set_rotation(manager, root, euler_to_quat(scene->root->get_rotation));


	import_scene_node(editor, scene, scene->root, root);
	return root;
}





bool init_editor_interface(EditorInterface* editor, EngineAPI api) {

	map_init(&editor->entity_selected);
	editor->show_editor = true;

	editor->show_info = true;
	editor->show_warning = true;
	editor->show_fatal = true;

	editor->log_start_offset = 0;

	editor->show_time = true;
	editor->show_tag = true;
	editor->show_thread_id = true;
	editor->show_filename = true;
	editor->show_function = true;
	editor->show_line = true;
	editor->show_message = true;


	editor->window_scene_tree_open = true;
	editor->window_log_open = true;
	editor->window_asset_browser_open = true;
	editor->window_entity_components_open = true;
	editor->window_engine_timers_open = true;
	editor->window_render_stats = true;
	

	editor->api = api;

	
	
	

	arena_init(&editor->arena);
	size_t mem_size = EDITOR_MEMORY;
	void* mem_block = arena_alloc(&editor->arena, mem_size);
	mem_size = editor->arena.end - cast(char*) mem_block;
	stack_alloc_init(&editor->stack, mem_block, mem_size);

	
	
	init_asset_importer(&editor->importer, &editor->api.asset_manager->asset_tracker);

	

	map_put(&editor->entity_selected, editor->api.entity_manager->root.id, false);

	editor->editor_camera = create_entity(api.entity_manager, "Editor Camera");
	//add_component(api.entity_manager, editor->editor_camera , ComponentType::Transform);
	add_component(api.entity_manager, editor->editor_camera , ComponentType::Camera);

	set_camera_params(api.entity_manager, editor->editor_camera, 0.0001f, 100.0f, 90.0f, api.window->size.x / cast(float) api.window->size.y);
	set_position(api.entity_manager, editor->editor_camera, Vec3f(0, 0, 0));

	
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");

	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);







	
	//AssetID tcolor = import_texture(editor->api.asset_manager,		String("Assets/textures/plastic/scuffed-plastic-alb.png"), true);
	//AssetID tnormal = import_texture(editor->api.asset_manager,		String("Assets/textures/plastic/scuffed-plastic-normal.png"), true);
	//AssetID tmetallic = import_texture(editor->api.asset_manager,	String("Assets/textures/plastic/scuffed-plastic-metal.png"), true);
	//AssetID troughness = import_texture(editor->api.asset_manager,	String("Assets/textures/plastic/scuffed-plastic-rough.png"), true);
	//AssetID tao = import_texture(editor->api.asset_manager,			String("Assets/textures/plastic/scuffed-plastic-ao.png"), true);
	////AssetID theight = import_texture(editor->api.asset_manager, String("Assets/textures/wet_stone/slipperystonework-height.png"), true);
	//
	//Material test_mat;
	//init_material_defaults(&test_mat);
	//
	//test_mat.albedo = tcolor.texture;
	//test_mat.normal = tnormal.texture;
	//test_mat.metal = tmetallic.texture;
	//test_mat.roughness = troughness.texture;
	//test_mat.ao = tao.texture;
	////test_mat.height = theight.texture;
	//
	//AssetID mat_id = create_material_asset(editor->api.asset_manager, "Assets/textures/plastic/", "plastic", &test_mat);
	//editor->test_mat = mat_id;

	

	//AssetID mat_id = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/bamboo-wood/bamboo-wood_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_green_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/wet_stone/slipperystonework_mat.easset");
	editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/gold/gold_mat.easset");

	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/rust_iron/rust_iron_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/paint_cement/paint_cement_mat_mat.easset");
	
	


	EntityManager* entity_manager = editor->api.entity_manager;
	AssetManager* asset_manager = editor->api.asset_manager;

	Entity e3 = create_entity(entity_manager, "Entity 3");
	Entity e4 = create_entity(entity_manager, "Entity 4");
	Entity e5 = create_entity(entity_manager, "Entity 5");
	Entity e6 = create_entity(entity_manager, "Entity 6");
	Entity e7 = create_entity(entity_manager, "Entity 7");
	Entity e8 = create_entity(entity_manager, "Entity 8");
	editor->test_mesh = create_entity(entity_manager, "Test mesh");
	editor->entity_test_light = create_entity(entity_manager, "Test Light");

	add_component(entity_manager, editor->entity_test_light, ComponentType::Light);


	attach_child_entity(entity_manager, e3, e4);
	attach_child_entity(entity_manager, e3, e8);
	attach_child_entity(entity_manager, e4, e5);
	attach_child_entity(entity_manager, e3, e6);
	attach_child_entity(entity_manager, e3, editor->test_mesh);

	

	map_put(&editor->entity_selected, editor->editor_camera.id, false);
	map_put(&editor->entity_selected, e3.id, false);
	map_put(&editor->entity_selected, e4.id, false);
	map_put(&editor->entity_selected, e5.id, false);
	map_put(&editor->entity_selected, e6.id, false);
	map_put(&editor->entity_selected, e7.id, false);
	map_put(&editor->entity_selected, e8.id, false);
	map_put(&editor->entity_selected, editor->test_mesh.id, false);
	map_put(&editor->entity_selected, editor->entity_test_light.id, false);

	



	
	

	add_component(entity_manager, editor->test_mesh, ComponentType::StaticMesh);
	add_component(entity_manager, editor->test_mesh, ComponentType::Render);
	set_render_material(entity_manager, editor->test_mesh, editor->test_mat.material);
	
	set_static_mesh(entity_manager, editor->test_mesh, editor->api.asset_manager->monkey_mesh.mesh);


	add_component(entity_manager, e3, ComponentType::StaticMesh);
	add_component(entity_manager, e3, ComponentType::Render);
	set_render_material(entity_manager, e3, editor->test_mat.material);
	set_static_mesh(entity_manager, e3, editor->api.asset_manager->sphere_mesh.mesh);

	set_position(entity_manager, editor->test_mesh, Vec3f(5, 0, 0));

	//add_component(entity_manager, editor->test_mesh, ComponentType::StaticMesh);
	//add_component(entity_manager, editor->test_mesh, ComponentType::Render);
	//set_render_material(entity_manager, editor->test_mesh, mat_id.material);
	//set_render_visibility(entity_manager, editor->test_mesh, true);
	//set_static_mesh(entity_manager, editor->test_mesh, editor->api.asset_manager->cube_mesh.mesh);
	


	
	
	


	Light light;
	light.type = LightType::DirectionalLight;
	light.dir_light.direction = Vec3f(0.00000000001f, -1.0f, 0.0f);;
	light.dir_light.color = Vec3f(1, 1, 1);
	set_light(entity_manager, editor->entity_test_light, light);




	
	

	//AssetImporter* importer = &editor->importer;
	//AssetID scene_asset;
	//scene_asset = find_asset_by_name(importer->tracker, "mill.fbx.easset");
	//if (scene_asset.id == 0) {
	//	scene_asset = import_fbx(importer, "Assets/test_fbx/mill.fbx", true);
	//}
	

	return true;
}

void destroy_editor_interface(EditorInterface* editor) {
	map_destroy(&editor->entity_selected);
	destroy_asset_importer(&editor->importer);
	arena_free(&editor->arena);
}




void editor_update(EditorInterface* editor) {

	
	Input* input = editor->api.input;
	GameTimer* timer = editor->api.game_loop;
	EntityManager* entity_manager = editor->api.entity_manager;

	Renderer* renderer = editor->api.renderer;
	Window* window = editor->api.window;

	
	

	

	// Need to move key presses to use repeats and add key modifiers
	if (is_key_pressed(input, KEYCODE_Z) && is_key_down(input, KEYCODE_LCTRL)) {
		perform_undo_operation(editor);
	}


	if (is_key_pressed(input, KEYCODE_Y) && is_key_down(input, KEYCODE_LCTRL)) {
		perform_redo_operation(editor);
	}

	if (is_key_pressed(input, KEYCODE_C) && is_key_down(input, KEYCODE_LCTRL)) {
		LOG_INFO("Editor", "perform copy command");
	}
	if (is_key_pressed(input, KEYCODE_V) && is_key_down(input, KEYCODE_LCTRL)) {
		LOG_INFO("Editor", "perform paste command");
	}
	if (is_key_pressed(input, KEYCODE_D) && is_key_down(input, KEYCODE_LCTRL)) {
		LOG_INFO("Editor", "perform duplicate command");
	}

	if (is_key_pressed(input, KEYCODE_BACKQUOTE)) {
		editor->show_editor = !editor->show_editor;
	}

	

	if (editor->show_editor) {

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruDockspace;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		//ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);

		} else {
			
		}
		ImGui::End();


		


	


		draw_main_menu_bar(editor);
		draw_window_entity_components(editor);
		draw_window_scene_hierarchy(editor);

		draw_window_engine_timer(editor);
		draw_window_log(editor);
		draw_window_assets(editor);
		//draw_window_scene_viewports(editor);

		draw_window_renderer_stats(editor);
		draw_editor_command_undo_and_redo_stack(editor);

		
		
	
	} // END OF SHOW EDITOR

	

	
	


	

	

	if (is_mouse_pressed(input, MouseButton::Left)) {
		Vec2i mouse_pos = get_mouse_pos(input);

		// Do a raycast to check which object is pressed
		//LOG_INFO("MOUSE", "Mouse pos %d, %d\n", mouse_pos.x, mouse_pos.y);
	}
	




	float delta_time = timer->delta_time;
	Camera* camera = get_camera(entity_manager, editor->editor_camera);




	Vec2i scroll = get_scroll_delta(input);

	
	
	// Capture scolling to move camera forward and back
	if (scroll.y != 0) {
		Vec3f new_cam_direction = (delta_time * -forward(entity_manager, editor->editor_camera));

		// TODO: make this configurable
		float scroll_scale = 10.0f;

		float cam_move_scale = scroll_scale * scroll.y;
		Vec3f cam_pos = get_position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));
	}





	int x = input->mouse.pos.x;
	int y = input->mouse.pos.y;

	int gx = input->mouse.global_pos.x;
	int gy = input->mouse.global_pos.y;

	int sx = window->size.x;
	int sy = window->size.y;

	Vec2i delta_pos = input->mouse.delta_pos;

	


	// Only apply editor movement if right mouse button is clicked
	if (is_mouse_down(input, MouseButton::Right)) {
		//SDL_SetWindowGrab(window->sdl_window, SDL_TRUE);
		//SDL_ShowCursor(SDL_DISABLE);
		//
		SDL_SetRelativeMouseMode(SDL_TRUE);
		//SDL_CaptureMouse(SDL_TRUE);

		Vec2i rel_pos = Vec2i(0, 0);


		SDL_GetRelativeMouseState(&rel_pos.x, &rel_pos.y);


		// TODO: remove need for sdl specific scan codes. convert to our own input api

		Vec3f new_cam_direction;


		// Since the camera always looks down -z
		if (is_key_down(input, KEYCODE_W)) { new_cam_direction += (delta_time * -forward(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_S)) { new_cam_direction += (delta_time * forward(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_D)) { new_cam_direction += (delta_time * right(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_A)) { new_cam_direction += (delta_time * -right(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_LSHIFT)) { new_cam_direction += (delta_time * up(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_LCTRL)) { new_cam_direction += (delta_time * -up(entity_manager, editor->editor_camera)); }

		float cam_move_scale = 10;
		Vec3f cam_pos = get_position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));


		// Prevent camera rotation jump, when the delta between the last time the right mouse was down and now
		if (editor->was_last_frame_using_right_click) {
			// TODO: this will be exposed to the user, we still need to implement proper control handling in engine
			float sensitivity = 0.25f;
			Quat old_cam_rot = get_rotation(entity_manager, editor->editor_camera);
			Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -rel_pos.x * sensitivity) * old_cam_rot;
			new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -rel_pos.y * sensitivity);

			set_rotation(entity_manager, editor->editor_camera, new_cam_rot);
		}
		


		editor->was_last_frame_using_right_click = true;
	} else {
		editor->was_last_frame_using_right_click = false;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
		
	process_editor_command_buffer(editor);
}




static bool is_entity_selected(EditorInterface* editor, Entity entity) {
	bool selected = map_get(&editor->entity_selected, entity.id, false);
	return selected;
}


static void draw_main_menu_bar(EditorInterface* editor) {
	// Menu bar
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Project", "Ctrl+N")) { /* Do stuff */ }
			if (ImGui::MenuItem("Open Project", "Ctrl+O")) {}
			if (ImGui::MenuItem("Build", "Ctrl+B")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Exit")) {
				
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene")) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				cmd_editor_create_emtpy_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				cmd_editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				cmd_editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				cmd_editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				cmd_editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				cmd_editor_create_sphere(editor);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assets")) {

			if (ImGui::MenuItem("Import")) {}
			if (ImGui::MenuItem("Create Material")) {}
			if (ImGui::MenuItem("Create Folder")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {

			
			
			ImGui::MenuItem("Scene Tree", NULL, &editor->window_scene_tree_open);
			ImGui::MenuItem("Entity Components", NULL, &editor->window_entity_components_open);
			ImGui::MenuItem("Game Loop", NULL, &editor->window_engine_timers_open);
			ImGui::MenuItem("Log", NULL, &editor->window_log_open);
			ImGui::MenuItem("Asset Browser", NULL, &editor->window_asset_browser_open);
			ImGui::MenuItem("Render Stats", NULL, &editor->window_render_stats);
			
			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

}

static void draw_component_transform(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	
	
	
	if (ImGui::CollapsingHeader("Transform")) {
		Vec3f old_pos = get_position(entity_manager, e);
		Vec3f new_pos = old_pos;

		Quat old_rot = get_rotation(entity_manager, e);
		Vec3f euler = quat_to_euler(old_rot);
		Quat new_rot = old_rot;

		Vec3f old_scale = get_scale(entity_manager, e);
		Vec3f new_scale = old_scale;

		
		
		

		ImGui::PushID("pos_default");
		if (ImGui::SmallButton("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, Vec3f(0, 0, 0), old_rot, old_scale, true);
		}
		ImGui::SameLine();

		
		
		if (ImGui::DragFloat3("Position", new_pos.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			set_position(entity_manager, e, new_pos);

			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, new_pos, old_rot, old_scale, true);
		}
		

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}
		
		
		
		ImGui::PopID();
		
		ImGui::PushID("rot_default");

		if (ImGui::SmallButton("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, Quat(), old_scale, false);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Rotation", euler.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			new_rot = euler_to_quat(euler);
			set_rotation(entity_manager, e, new_rot);			
			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, new_rot, old_scale, true);
		}

	

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		

		ImGui::PopID();


		ImGui::PushID("scale_default");
		
		if (ImGui::SmallButton("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, old_rot, Vec3f(1, 1, 1), false);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Scale", new_scale.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			set_scale(entity_manager, e, new_scale);
			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, old_rot, new_scale, true);
		}


		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PopID();
		

		
		
		
	} else {
		
	}

	
}


static void draw_component_camera(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;

	bool open = true;
	if (ImGui::CollapsingHeader("Camera", &open)) {
		bool comp_enabled = true;
		ImGui::Checkbox("Enabled", &comp_enabled);
			
		
		
		const Camera* const old_cam = get_camera(entity_manager, e);
		Camera new_camera = *old_cam;
		//ImGui::DragFloat("Aspect Ratio", &cam->aspect_ratio);

		int projection = (int)old_cam->projection;
		if (ImGui::Combo("Projection", &projection, "Perspective\0Orthographic\0\0")) {
			new_camera.projection = (CameraProjection)projection;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, false);
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PushID("camera_fov_default");
		if (ImGui::SmallButton("z")) {
			new_camera.fov = 90.0f;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);

		}
		ImGui::SameLine();
		if (ImGui::SliderFloat("Field Of View", &new_camera.fov, 30.0f, 120.0f)) {
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}
		ImGui::PopID();

		ImGui::PushID("camera_near_default");
		if (ImGui::SmallButton("z")) {
			new_camera.near_clip = 0.01f;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat("Near", &new_camera.near_clip, 1.0f, 0.0f, FLT_MAX)) {
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PopID();

		ImGui::PushID("camera_far_default");
		if (ImGui::SmallButton("z")) {
			new_camera.far_clip = 100.0f;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}
		ImGui::SameLine();

		if (ImGui::DragFloat("Far", &new_camera.far_clip, 1.0f, 0.0f, FLT_MAX)) {
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PopID();


		if (old_cam->projection == CameraProjection::Orthographic) {

			//ImGui::Separator();
			//ImGui::Text("Orthographic Settings");

			ImGui::PushID("camera_left_default");
			if (ImGui::SmallButton("z")) {
				new_camera.left = 0.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Left", &new_camera.left, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_right_default");
			if (ImGui::SmallButton("z")) {
				new_camera.right = 100.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Right", &new_camera.right, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}

			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_top_default");
			if (ImGui::SmallButton("z")) {
				new_camera.top = 0.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Top", &new_camera.top, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_bottom_default");
			if (ImGui::SmallButton("z")) {
				new_camera.bottom = 100.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();

			if (ImGui::DragFloat("Bottom", &new_camera.bottom, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();
		}

		set_camera(entity_manager, e, new_camera);
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::Camera);
	}
}


static void draw_component_light(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	bool open = true;
	if (ImGui::CollapsingHeader("Light", &open)) {
		
		bool comp_enabled = is_component_enabled(entity_manager, e, ComponentType::Light);
		ImGui::PushID("light_enabled");
		if (ImGui::Checkbox("Enabled", &comp_enabled)) {
			enable_component(editor->api.entity_manager, e, ComponentType::Light, comp_enabled);
		}
		ImGui::PopID();
			
		
		
		

		const Light old_light =  get_light(entity_manager, e);
		Light new_light = old_light;
		int light_type = (int)old_light.type;
		if (ImGui::Combo("Type", &light_type, "None\0Directional Light\0Point Light\0\0")) {
			new_light.type = (LightType)light_type;
			cmd_editor_set_light_component(editor, e, old_light, new_light, true);
		}

		switch (old_light.type) {
			case LightType::DirectionalLight: {


				ImGui::PushID("dir_direction_default");
				if (ImGui::SmallButton("z")) {
					new_light.dir_light.direction = Vec3f(0, 0, 0);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();
				if (ImGui::DragFloat3("Direction", new_light.dir_light.direction.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}

				ImGui::PopID();

				ImGui::PushID("dir_color_default");
				if (ImGui::SmallButton("z")) {
					new_light.dir_light.color = Vec3f(1, 1, 1);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				ImGui::SameLine();
				if (ImGui::ColorEdit3("Color", new_light.dir_light.color.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();

				

				ImGui::PushID("dir_intensity_default");
				if (ImGui::SmallButton("z")) {
					new_light.dir_light.intensity = 0;
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();

				if (ImGui::DragFloat("Intensity", &new_light.dir_light.intensity, 1.0f, 0.0f, 100000.0f)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();

				

				break;
			}
			case LightType::PointLight: {
				ImGui::PushID("point_color_default");
				if (ImGui::SmallButton("z")) {
					new_light.point_light.color = Vec3f(1,1,1);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Color", new_light.point_light.color.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();
				break;
			}
		}

		set_light(entity_manager, e, new_light);
		
		
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Light);
	}
}

static void draw_component_static_mesh(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;

	bool open = true;
	if (ImGui::CollapsingHeader("Static Mesh", &open)) {
		StaticMeshID mesh_id = get_static_mesh(entity_manager, e);
		AssetID assetid;
		assetid.mesh = mesh_id;
		String name = name_of_asset(&editor->api.asset_manager->asset_tracker, assetid);
		ImGui::Text(name.buffer);

		if (ImGui::Button("Select Mesh")) {
			ImGui::OpenPopup("Select Mesh");
		}
		
		
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		if (ImGui::BeginPopup("Select Mesh", NULL))
		{
			
			AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
			size_t map_size = tracker->track_map.size;
			// Go over our track map, and look for filename
			for (size_t i = 0; i < map_size; i++) {
				CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
				// Check if this is a valid track data
				if (track_item.key != 0 && track_item.key != TOMBSTONE) {

					// Only show Static mesh assets
					if (track_item.value.assetid.type == AssetType::StaticMesh) {
						if (ImGui::Button(track_item.value.file.buffer)) {
							cmd_editor_set_staticmesh_component(editor, e, mesh_id, track_item.value.assetid.mesh);
							
						}
					}
				}
			}

			
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::StaticMesh);
	}
}

static void draw_component_render(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	bool open = true;
	if (ImGui::CollapsingHeader("Render", &open)) {
		bool comp_enabled = is_component_enabled(entity_manager, e, ComponentType::Render);
		ImGui::PushID("render_enabled");
		if (ImGui::Checkbox("Enabled", &comp_enabled)) {
			enable_component(editor->api.entity_manager, e, ComponentType::Render, comp_enabled);
		}
		ImGui::PopID();



		MaterialID mat_id = get_render_material(entity_manager, e);
		AssetID assetid;
		assetid.material = mat_id;
		String name = name_of_asset(&editor->api.asset_manager->asset_tracker, assetid);
		ImGui::Text(name.buffer);

		if (ImGui::Button("Select Material")) {
			ImGui::OpenPopup("Select Material");
		}

		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		if (ImGui::BeginPopup("Select Material", 0))
		{

			AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
			size_t map_size = tracker->track_map.size;
			// Go over our track map, and look for filename
			for (size_t i = 0; i < map_size; i++) {
				CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
				// Check if this is a valid track data
				if (track_item.key != 0 && track_item.key != TOMBSTONE) {

					if (track_item.value.assetid.type == AssetType::Material) {
						if (ImGui::Button(track_item.value.file.buffer)) {
							cmd_editor_set_material_component(editor, e, mat_id, track_item.value.assetid.material);
						}
					}
				}
			}


			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Render);
	}
}



static void draw_window_entity_components(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	if (!editor->window_entity_components_open) {
		return;
	}

	if (ImGui::Begin("Entity Components", &editor->window_entity_components_open)) {
		
		

		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
			MapResult<bool> result = map_get(&editor->entity_selected, e.id);
			if (!result.found) continue;
			if (result.value) {
				ImGui::PushID(e.id);
				String e_name = get_name(&entity_manager->meta_manager, e);



				//ImGui::InputText("", e_name.buffer, e_name.length);
				//ImGui::SameLine();
				ImGui::Text(e_name.buffer);
				
				//ImGui::SameLine();


				
				ComponentType comp_types[] = {
					ComponentType::Transform,
					ComponentType::Camera,
					ComponentType::StaticMesh,
					ComponentType::Light,
					ComponentType::Render
				};

				const char* comp_names[] = { "Transform", "Camera", "StaticMesh", "Light", "Render" };
				

			

				bool enabled = 1;
				if (ImGui::Checkbox("Enabled", &enabled)) {
					
				}
				ImGui::SameLine();
				if (ImGui::Button("Add Component")) {
					ImGui::OpenPopup("component_popup");
				}
				ImGui::SameLine();
				if (ImGui::BeginPopup("component_popup")) {
					
					for (int i = 0; i < IM_ARRAYSIZE(comp_names); i++)
						if (ImGui::Selectable(comp_names[i])) {
							
							add_component(entity_manager, e, comp_types[i]);
						}
							
					ImGui::EndPopup();
				}
				ImGui::NewLine();

				if (has_component(entity_manager, e, ComponentType::Transform)) {
					draw_component_transform(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::Camera)) {
					draw_component_camera(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::Light)) {
					draw_component_light(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::StaticMesh)) {
					draw_component_static_mesh(editor, e);
				}
				if (has_component(entity_manager, e, ComponentType::Render)) {
					draw_component_render(editor, e);
				}
				ImGui::PopID();




				ImGui::Separator();
				ImGui::Spacing();

			}





		}
	}
	ImGui::End();

	//if (ImGui::Begin("Render Components")) {
	//	RenderManager* rm = &entity_manager->render_manager;
	//	ImGui::Text("Capacity %d", rm->total_count);
	//	ImGui::Text("Enabled %d", rm->enabled_count);
	//
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%d ", rm->entitys[i].id);
	//		ImGui::SameLine();
	//	}
	//	ImGui::NewLine();
	//	for (u64 i = 0; i < rm->total_count; i++) {
	//		ImGui::Text("%d ", rm->entitys[i].id);
	//		ImGui::SameLine();
	//	}
	//
	//	ImGui::NewLine();
	//	size_t map_size = rm->id_map.size;
	//	// Go over our track map, and look for filename
	//	for (int i = 0; i < map_size; i++) {
	//		CompactMapItem<u64> item = rm->id_map.map[i];
	//		
	//		// Check if this is a valid track data
	//		if (item.key!= 0 && item.key != TOMBSTONE) {
	//			ImGui::Text("%llu -> %llu", item.key, item.value);
	//		}
	//	}
	//
	//}
	//ImGui::End();
		

}

static void draw_entity_item_context_menu(EditorInterface* editor, Entity e) {
	if (ImGui::BeginPopupContextItem("Entity Item Context Menu")) {


		if (ImGui::Selectable("Copy")) {

		}
		if (ImGui::Selectable("Paste")) {

		}

		if (ImGui::Selectable("Duplicate")) {

		}
		if (ImGui::Selectable("Rename")) {

		}
		ImGui::Separator();
		if (ImGui::Selectable("Delete")) {
			EditorCommand command;
			command.type = EditorCommandType::DELETE_ENTITY;
			command.cmd.delete_entity.entity_to_delete = e;
			push_editor_command(editor, command);
		}


		ImGui::EndPopup();
	}
}

static void draw_entity_tree(EditorInterface* editor, Entity e) {

	//if (editor->editor_camera == e) {
	//	// Dont draw the editor camera in the tree
	//	return;
	//}


	bool entity_selected = is_entity_selected(editor, e);

	auto em = editor->api.entity_manager;
	// We need to recursivly draw the children
	Entity child = first_child(em, e);


	String name = get_name(&em->meta_manager, e);
	
	ImGui::PushID(e.id);
	if (child.id == NO_ENTITY_ID) {
		// This is a leaf node
		ImGui::Indent();
		
		if (ImGui::Selectable(name.buffer, entity_selected)) {
			cmd_editor_group_begin(editor);
			if (!ImGui::GetIO().KeyCtrl) {
				cmd_editor_deselect_all_entitys(editor);
			}
			cmd_editor_select_entity(editor, e.id, !entity_selected);
			cmd_editor_group_end(editor);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}
		draw_entity_item_context_menu(editor, e);

		

		ImGui::Unindent();
	} else {


		// This is a node that has children

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((entity_selected) ? ImGuiTreeNodeFlags_Selected : 0);

		bool node_open = ImGui::TreeNodeEx(name.buffer, node_flags);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}
		draw_entity_item_context_menu(editor, e);


		if (ImGui::IsItemClicked()) {
			cmd_editor_group_begin(editor);
			if (!ImGui::GetIO().KeyCtrl) {
				// Clear selection when CTRL is not held
				cmd_editor_deselect_all_entitys(editor);
			}
			cmd_editor_select_entity(editor, e.id, !entity_selected);
			cmd_editor_group_end(editor);

		}
		if (node_open) {
			while (child.id != NO_ENTITY_ID) {
				draw_entity_tree(editor, child);
				child = next_sibling(em, child);
			}
			ImGui::TreePop();
		}


	}
	ImGui::PopID();


}

static void draw_window_scene_hierarchy(EditorInterface* editor) {
	if (!editor->window_scene_tree_open) {
		return;
	}
	EntityManager* entity_manager = editor->api.entity_manager;
	if (ImGui::Begin("Entity Scene Tree", &editor->window_scene_tree_open, ImGuiWindowFlags_DockNodeHost)) {
		
		
		
		ImGui::Text("Total Entitys Created %llu", entity_manager->entitys_created);
		ImGui::Text("Current Entity Count %llu", entity_manager->entity_count);
		ImGui::Text("Entity Selected Count %llu", editor->entity_selected_count);
		ImGui::Separator();


		//static char str0[128] = "";
		//ImGui::Text("Filter Entitys");
		//ImGui::InputText("", str0, IM_ARRAYSIZE(str0));
		

		editor->scene_tree_entity_filter.Draw("Filter");

		ImGui::Separator();

		
		

		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
		
			String name = get_name(&entity_manager->meta_manager, e);
			const char* start = name.buffer;
			const char* end = name.buffer + name.length;
			if (!editor->scene_tree_entity_filter.PassFilter(name.buffer, name.buffer + name.length)) {
				// TODO: children filtering
				continue;
			}
		
		
			Entity parent_entity = parent(entity_manager, e);
		
			if (parent_entity == entity_manager->root) {
				draw_entity_tree(editor, e);
			}
			
		}

		ImGui::BeginChild("scene_internal");

		if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(0, false)) {
			cmd_editor_deselect_all_entitys(editor);
		}
		

		if (ImGui::BeginPopupContextWindow()) {
			

			if (ImGui::MenuItem("Copy")) {
			
			}

			if (ImGui::MenuItem("Paste")) {
				
			}
			if (ImGui::MenuItem("Duplicate")) {

			}

			

			ImGui::Separator();

			if (ImGui::MenuItem("Create Empty Entity")) {
				cmd_editor_create_emtpy_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				cmd_editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				cmd_editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				cmd_editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				cmd_editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				cmd_editor_create_sphere(editor);
			}
			ImGui::EndPopup();
		}

		ImGui::EndChild();
		

	}
	ImGui::End();

	//if (ImGui::Begin("Transform Components")) {
	//	TransformManager* rm = &entity_manager->transform_manager;
	//	ImGui::Text("Capacity %d", rm->total_count);
	//	ImGui::Text("Enabled %d", rm->enabled_count);
	//
	//	//for (u64 i = 0; i < rm->enabled_count; i++) {
	//	//	ImGui::Text("%d ", rm->entitys[i].id);
	//	//	ImGui::SameLine();
	//	//}
	//	//ImGui::NewLine();
	//	//for (u64 i = 0; i < rm->capacity; i++) {
	//	//	ImGui::Text("%d ", rm->entitys[i].id);
	//	//	ImGui::SameLine();
	//	//}
	//	//
	//	//ImGui::NewLine();
	//	//size_t map_size = rm->id_map.size;
	//	//// Go over our track map, and look for filename
	//	//for (int i = 0; i < map_size; i++) {
	//	//	CompactMapItem<u64> item = rm->id_map.map[i];
	//	//
	//	//	// Check if this is a valid track data
	//	//	if (item.key != 0 && item.key != TOMBSTONE) {
	//	//		ImGui::Text("%llu -> %llu", item.key, item.value);
	//	//	}
	//	//}
	//
	//	ImGui::Text("Parents");
	//	ImGui::SameLine();
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%llu ", rm->parent[i]);
	//		ImGui::SameLine();
	//	}
	//	ImGui::NewLine();
	//
	//	ImGui::Text("1childs");
	//	ImGui::SameLine();
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%llu ", rm->first_child[i]);
	//		ImGui::SameLine();
	//	}
	//	ImGui::NewLine();
	//	ImGui::Text("Nxtsibs");
	//	ImGui::SameLine();
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%llu ", rm->next_sibling[i]);
	//		ImGui::SameLine();
	//	}
	//	ImGui::NewLine();
	//	ImGui::Text("Prvsibs");
	//	ImGui::SameLine();
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%llu ", rm->prev_sibling[i]);
	//		ImGui::SameLine();
	//	}
	//
	//}
	//ImGui::End();
	
}


static void draw_window_engine_timer(EditorInterface* editor) {
	GameTimer* timer = editor->api.game_loop;
	if (!editor->window_engine_timers_open) {
		return;
	}
	
	if (ImGui::Begin("Game Loop", &editor->window_engine_timers_open)) {

		ImGui::Checkbox("Cap framerate", &timer->cap_framerate);
		if (timer->cap_framerate) {
			ImGui::SliderInt("Target Framerate", &timer->target_fps, 0, 240);
		}
		editor->fps_history[editor->fps_history_index] = timer->fps;
		editor->fps_history_index = ((editor->fps_history_index + 1) % FPS_HISTORY_COUNT);
		static char fps_text[32];
		snprintf(fps_text, 32, "FPS %d - dt %f", timer->fps, timer->delta_time);
		ImGui::PlotLines("Frame Times", editor->fps_history, IM_ARRAYSIZE(editor->fps_history), 0, fps_text, 0.0f, 120.0f, ImVec2(0, 140));


		ImGui::Text("Time %f", timer->seconds);
		ImGui::Text("Physics Time %f", timer->physics_time);
		ImGui::Text("Physics Time Step %f", timer->time_step);

		ImGui::Text("Frame Count %d", timer->frame_count);
		ImGui::Text("Ticks %llu", timer->ticks);
		ImGui::Text("Delta Ticks %llu", timer->delta_ticks);
		ImGui::Text("Ticks per sec %llu", timer->ticks_per_sec);

	}
	ImGui::End();

}

static void draw_window_log(EditorInterface* editor) {

	if (!editor->window_log_open) {
		return;
	}

	if (ImGui::Begin("Log", &editor->window_log_open)) {

		LogList logs = g_get_loglist();

		editor->log_filter.Draw("Filter");
		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			editor->log_start_offset = logs.log_count;
		}

		ImGui::Checkbox("Auto Scroll", &editor->auto_scroll);


		ImGui::Checkbox("Info", &editor->show_info);
		ImGui::SameLine();
		ImGui::Checkbox("Warning", &editor->show_warning);
		ImGui::SameLine();
		ImGui::Checkbox("Fatal", &editor->show_fatal);


		ImGui::Text("Filter Columns");
		ImGui::SameLine();

		
		



		static const char* str_time = "Time";
		static const char* str_tag = "Tag";
		static const char* str_thread_id = "Thread Id";
		static const char* str_filename = "Filename";
		static const char* str_function = "Function";
		static const char* str_line = "Line";
		static const char* str_msg = "Message";


		ImGui::Checkbox(str_time, &editor->show_time); ImGui::SameLine();
		ImGui::Checkbox(str_tag, &editor->show_tag); ImGui::SameLine();
		ImGui::Checkbox(str_thread_id, &editor->show_thread_id); ImGui::SameLine();
		ImGui::Checkbox(str_filename, &editor->show_filename); ImGui::SameLine();
		ImGui::Checkbox(str_function, &editor->show_function); ImGui::SameLine();
		ImGui::Checkbox(str_line, &editor->show_line); ImGui::SameLine();
		ImGui::Checkbox(str_msg, &editor->show_message); ImGui::SameLine();

		ImGui::Spacing();
		ImGui::Separator();
		int col_count =
			editor->show_time
			+ editor->show_tag
			+ editor->show_thread_id
			+ editor->show_filename
			+ editor->show_function
			+ editor->show_line
			+ editor->show_message;

		ImGui::BeginGroup();
		ImGui::BeginChild(1);
		if (col_count != 0) {
			ImGui::Columns(col_count, "log_columns");
			if (editor->show_time) {
				ImGui::Text(str_time); ImGui::NextColumn();
			}
			if (editor->show_tag) {
				ImGui::Text(str_tag); ImGui::NextColumn();
			}
			if (editor->show_thread_id) {
				ImGui::Text(str_thread_id); ImGui::NextColumn();
			}
			if (editor->show_filename) {
				ImGui::Text(str_filename); ImGui::NextColumn();
			}
			if (editor->show_function) {
				ImGui::Text(str_function); ImGui::NextColumn();
			}
			if (editor->show_line) {
				ImGui::Text(str_line); ImGui::NextColumn();
			}

			if (editor->show_message) {
				ImGui::Text(str_msg); ImGui::NextColumn();
			}
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

			
				for (int i = editor->log_start_offset; i < logs.log_count; i++) {
					LogItem* log_item = &logs.logs[i];
					
					bool found = false;

					if (editor->log_filter.PassFilter(log_item->msg, log_item->msg + log_item->msg_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->tag, log_item->tag + log_item->tag_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->function, log_item->function + log_item->function_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->filename, log_item->filename + log_item->filename_length)) {
						found = true;
					}
					if (!found) {
						continue;
					}
					
					
					bool pop_color = false;

					const char* log_type_str;
					switch (log_item->verbosity) {
						case LoggerVerbosity::INFO: {
							if (!editor->show_info) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); pop_color = true;
							log_type_str = "[INFO]"; break;
						}
						case LoggerVerbosity::WARN: {
							if (!editor->show_warning) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.90f, 0.25f, 1.0f)); pop_color = true;
							log_type_str = "[WARN]"; break;
						}
						case LoggerVerbosity::FATAL: {
							if (!editor->show_fatal) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true;
							log_type_str = "[FATAL]"; break;
						}
						case LoggerVerbosity::VERBOSE: {
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); pop_color = true;
							log_type_str = "[VERBOSE]"; break;
						}
						default: {
							log_type_str = "[NONE]"; break;
						}
					}

					if (editor->show_time) {
						ImGui::Text("%llu", log_item->time);
						ImGui::NextColumn();
					}
					if (editor->show_tag) {
						ImGui::Text(log_item->tag);
						ImGui::NextColumn();
					}
					if (editor->show_thread_id) {
						ImGui::Text("%d", log_item->thread_id);
						ImGui::NextColumn();
					}
					if (editor->show_filename) {
						ImGui::TextUnformatted(log_item->filename, log_item->filename + log_item->filename_length - 1);
						ImGui::NextColumn();
					}
					if (editor->show_function) {
						ImGui::TextUnformatted(log_item->function, log_item->function + log_item->function_length - 1);
						ImGui::NextColumn();
					}
					if (editor->show_line) {
						ImGui::Text("%d", log_item->line);
						ImGui::NextColumn();
					}

					if (editor->show_message) {
						const char* end = (log_item->msg + log_item->msg_length - 1);
						ImGui::TextUnformatted(log_item->msg, end);
						ImGui::NextColumn();
					}


					if (pop_color) {
						ImGui::PopStyleColor();
					}

					ImGui::Separator();
				}
			
			


			ImGui::PopStyleVar();

			if (editor->auto_scroll) {
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::Columns(1);
			ImGui::EndChild();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

static void draw_window_assets(EditorInterface* editor) {

	if (!editor->window_asset_browser_open) {
		return;
	}

	if (ImGui::Begin("Asset Browser", &editor->window_asset_browser_open)) {

		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {  }
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {  }
		//ImGui::SameLine(0.0f, spacing);
		
		ImGui::Button("Assets");
		ImGui::SameLine(0.0f, spacing);
		ImGui::SmallButton("<");
		ImGui::SameLine(0.0f, spacing);
		ImGui::Button("textures");
		

		AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
		size_t map_size = tracker->track_map.size;
		// Go over our track map, and look for filename
		for (int i = 0; i < map_size; i++) {
			CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
			// Check if this is a valid track data
			if (track_item.key != 0 && track_item.key != TOMBSTONE) {
				AssetID assetid = find_asset_by_name(tracker, track_item.value.file.buffer);
				ImGui::Text("%llu, %lu, %s", assetid.id, assetid.type, track_item.value.file.buffer);
			}
		}
	}
	ImGui::End();
}

static void draw_window_renderer_stats(EditorInterface* editor) {

	
	if (!editor->window_render_stats) {
		return;
	}

	if (ImGui::Begin("Render Stats", &editor->window_render_stats)) {
		Renderer* renderer = editor->api.renderer;

		char renderer_type_str[16];

		switch (renderer->type) {
			case BackenedRendererType::OpenGL: {
				snprintf(renderer_type_str, 16, "OpenGL");
				break;
			}
			default: {
				snprintf(renderer_type_str, 16, "Unknown");
				break;
			}

		}
		ImGui::Text("Renderer Backend: %s", renderer_type_str);
		ImGui::Text("Shader Count: %d", renderer->opengl.shader_count);
		ImGui::Text("Texture Count: %d", renderer->opengl.texture_count);
		ImGui::Text("VAO Count: %d", renderer->opengl.vao_count);
		ImGui::Text("VBO Count: %d", renderer->opengl.vbo_count);
		ImGui::Text("EBO Count: %d", renderer->opengl.ebo_count);
		ImGui::Text("FBO Count: %d", renderer->opengl.fbo_count);
		ImGui::Text("RBO Count: %d", renderer->opengl.rbo_count);

		ImGui::Separator();
		ImGui::Text("Render World");
		
		ImGui::Text("Mesh Count: %d", renderer->render_world.render_mesh_count);
		ImGui::Text("Mesh Capacity: %d", renderer->render_world.render_mesh_capacity);

		
		ImGui::Text("Render Resoure Count: %d", renderer->render_world.resources_count);
		ImGui::Text("Material Count: %d", renderer->render_world.material_res_count);
		//ImGui::Text("Mesh Capacity: %d", renderer->render_world.render_mesh_capacity);
		
		
			
	}
	ImGui::End();
}

static void draw_window_scene_viewports(EditorInterface* editor) {


	if (ImGui::Begin("Scene")) {
	}
	ImGui::End();

	if (ImGui::Begin("Game")) {
	}
	ImGui::End();

	if (ImGui::Begin("Multi Windows")) {
	}
	ImGui::End();

}

static void draw_editor_command_undo_and_redo_stack(EditorInterface* editor) {
	if (ImGui::Begin("Command Undo Stack")) {

		size_t undo_stack_count = editor->cmd_buffer.command_undo_stack_count;
		EditorCommand* undo_stack = editor->cmd_buffer.command_undo_stack;
		for (size_t i = 0; i < undo_stack_count; i++) {
			EditorCommand cmd = undo_stack[i];
			if (cmd.type == EditorCommandType::COMMAND_GROUP_START) {
				ImGui::Text("Cmd Group Start: ID %llu", cmd.cmd.group.stack_level);
			}
			else if (cmd.type == EditorCommandType::COMMAND_GROUP_END) {
				ImGui::Text("Cmd Group End: ID %llu", cmd.cmd.group.stack_level);
			} else {
				ImGui::Text("Cmd type %d", cmd.type);
			}
			
		}
	}
	ImGui::End();

	if (ImGui::Begin("Command Redo Stack")) {

		size_t redo_stack_count = editor->cmd_buffer.command_redo_stack_count;
		EditorCommand* redo_stack = editor->cmd_buffer.command_redo_stack;
		for (size_t i = 0; i < redo_stack_count; i++) {
			EditorCommand cmd = redo_stack[i];
			if (cmd.type == EditorCommandType::COMMAND_GROUP_START) {
				ImGui::Text("Cmd Group Start: ID %llu", cmd.cmd.group.stack_level);
			} else if (cmd.type == EditorCommandType::COMMAND_GROUP_END) {
				ImGui::Text("Cmd Group End: ID %llu", cmd.cmd.group.stack_level);
			} else {
				ImGui::Text("Cmd type %d", cmd.type);
			}
		}
	}
	ImGui::End();
}


static void push_editor_undo_command(EditorInterface* editor, const EditorCommand& command) {
	editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count] = command;
	editor->cmd_buffer.command_undo_stack_count++;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_undo_stack_count < EDITOR_COMMAND_UNDO_BUFFER_CAPACITY);
}

static void push_editor_command(EditorInterface* editor, const EditorCommand& command) {
	
	
	//editor->cmd_buffer.current_group.command_count++;
	editor->cmd_buffer.commands[editor->cmd_buffer.command_count] = command;
	editor->cmd_buffer.command_count++;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_count < EDITOR_COMMAND_BUFFER_CAPACITY);

	push_editor_undo_command(editor, command);

	// Reset redo stack
	editor->cmd_buffer.command_redo_stack_count = 0;

}



static void push_editor_redo_command(EditorInterface* editor, const EditorCommand& command) {
	editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count] = command;
	editor->cmd_buffer.command_redo_stack_count++;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_redo_stack_count < EDITOR_COMMAND_UNDO_BUFFER_CAPACITY);
}




static void perform_undo_operation(EditorInterface* editor) {
	if (editor->cmd_buffer.command_undo_stack_count > 0) {
		LOG_INFO("Editor", "perform undo command");

		// Pop last command off the undo stack
		EditorCommand last_command = editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count - 1];
		editor->cmd_buffer.command_undo_stack_count--;
		
		// Push undo onto the redo stack
		push_editor_redo_command(editor, last_command);
		

		// If this is a group command
		// we need to pop the entire group off
		if (last_command.type == EditorCommandType::COMMAND_GROUP_END) {

			// The stack might look something like 
			// Start Group 0
			//  cmd 1
			//	Start Group 1
			//		cmd 2
			//		cmd 3
			//		cmd 4
			//	End Group 1
			//	cmd 5
			//	cmd 6
			// End Group 0

			
			u64 stack_level = last_command.cmd.group.stack_level;
			
			EditorCommand* sub_command;
			do {
				// Keep popping off sub commands till we reach the end command that is at the same level as the current stack level
				sub_command = &editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count - 1];
				editor->cmd_buffer.command_undo_stack_count--;

				perform_command(editor, *sub_command, true);

				// Push undo onto the redo stack
				push_editor_redo_command(editor, *sub_command);
			} while (!(sub_command->type == EditorCommandType::COMMAND_GROUP_START && sub_command->cmd.group.stack_level == stack_level));
			


		} else {
			// This is a non group command
			perform_command(editor, last_command, true);
			
		}


		if (last_command.type == EditorCommandType::COMMAND_FORCE_NO_MERGE) {
			// pop this command
			// And perform the next operation
			perform_undo_operation(editor);
				
		}

		

	
		assert(editor->cmd_buffer.command_undo_stack_count >= 0);

	}
	
}

static void perform_redo_operation(EditorInterface* editor) {
	LOG_INFO("Editor", "perform redo command");


	if (editor->cmd_buffer.command_redo_stack_count > 0) {
		EditorCommand last_command = editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
		editor->cmd_buffer.command_redo_stack_count--;

		// Push undo onto the undo stack
		push_editor_undo_command(editor, last_command);

		


		if (last_command.type == EditorCommandType::COMMAND_GROUP_START) {
			u64 stack_level = last_command.cmd.group.stack_level;

			EditorCommand* sub_command;
			do {
				sub_command = &editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
				editor->cmd_buffer.command_redo_stack_count--;
				perform_command(editor, *sub_command, false);
				// Push undo onto the redo stack
				push_editor_undo_command(editor, *sub_command);

			} while (!(sub_command->type == EditorCommandType::COMMAND_GROUP_END && sub_command->cmd.group.stack_level == stack_level));

		} else {
			perform_command(editor, last_command, false);

		}


		// We need to peek the redo stack for the no merge command
		if (editor->cmd_buffer.command_redo_stack_count > 0) { // Check if that there is commands on the redo stack. prevent buffer underflow
			EditorCommand peek_next_command = editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
			if (peek_next_command.type == EditorCommandType::COMMAND_FORCE_NO_MERGE) {
				perform_redo_operation(editor);
			}
		}
		

		assert(editor->cmd_buffer.command_redo_stack_count >= 0);

	}

}


static void perform_command(EditorInterface* editor, EditorCommand command, bool undo) {
	
	switch (command.type) {

		case EditorCommandType::COMMAND_GROUP_START: {
			EditorCommandGroup group = command.cmd.group;
			break;
		}
		case EditorCommandType::COMMAND_GROUP_END: {
			EditorCommandGroup group = command.cmd.group;
			break;
		}
		case EditorCommandType::COMMAND_FORCE_NO_MERGE: {
			break;
		}
		case EditorCommandType::NEW_ENTITY: {

			EditorCommand_NewEntity new_entity_command = command.cmd.new_entity;
			if (undo) {
				

				destroy_entity(editor->api.entity_manager, new_entity_command.entity);
			} else {
				Entity e = create_entity(editor->api.entity_manager, new_entity_command.name);
				map_put(&editor->entity_selected, e.id, false);
				command.cmd.new_entity.entity = e;


				ComponentType comp_flags = new_entity_command.component_flags;

				for (int i = 1; i < (1 << (int)ComponentType::Count); i *= 2) {
					ComponentType component = ComponentType(i);
					bool comp_set = ((u64)comp_flags & (u64)component);
					if (comp_set) {
						add_component(editor->api.entity_manager, e, component);
					}
				}

				if (new_entity_command.staticmesh_id.id != 0) {
					set_static_mesh(editor->api.entity_manager, e, new_entity_command.staticmesh_id);
				}

				if (new_entity_command.material_id.id != 0) {
					set_render_material(editor->api.entity_manager, e, new_entity_command.material_id);
				}
			}
			break;
		}
		case EditorCommandType::DELETE_ENTITY: {
			destroy_entity(editor->api.entity_manager, command.cmd.delete_entity.entity_to_delete);
			break;
		}
		case EditorCommandType::SELECT_ENTITY: {

			
			EditorCommand_SelectEntity select_command = command.cmd.select_entity;
			Entity selected_entity = select_command.entity_to_select;
			bool selected = select_command.selected;



			if (undo) {
				bool already_selected = is_entity_selected(editor, selected_entity);
				if (!selected == already_selected) { return; }

				map_put(&editor->entity_selected, selected_entity.id, !selected);

				if (!selected) {
					assert(editor->entity_selected_count < ULLONG_MAX);
					editor->entity_selected_count++;
				} else {
					assert(editor->entity_selected_count > 0);
					editor->entity_selected_count--;
				}
			} else {
				bool already_selected = is_entity_selected(editor, selected_entity);
				if (selected == already_selected) { return; }

				map_put(&editor->entity_selected, selected_entity.id, selected);

				if (selected) {
					assert(editor->entity_selected_count < ULLONG_MAX);
					editor->entity_selected_count++;
				} else {
					assert(editor->entity_selected_count > 0);
					editor->entity_selected_count--;
				}
			}
			break;
		}
		case EditorCommandType::DUPLICATE_ENTITY: {
			break;
		}
		case EditorCommandType::SET_TRANSFORM_COMPONENT: {
			EditorCommand_SetTransformComponent set_transform = command.cmd.set_transform;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_position(entity_manager, set_transform.entity, set_transform.old_position);
				set_rotation(entity_manager, set_transform.entity, set_transform.old_rotation);
				set_scale(entity_manager, set_transform.entity, set_transform.old_scale);
				
			} else {
				
				set_position(entity_manager, set_transform.entity, set_transform.position);
				set_rotation(entity_manager, set_transform.entity, set_transform.rotation);
				set_scale(entity_manager, set_transform.entity, set_transform.scale);
			}
			break;
		}
		case EditorCommandType::SET_STATICMESH_COMPONENT: {
			EditorCommand_SetStaticMeshComponent set_staticmesh = command.cmd.set_staticmesh;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_static_mesh(entity_manager, set_staticmesh.entity, set_staticmesh.old_id);
			}
			else {
				set_static_mesh(entity_manager, set_staticmesh.entity, set_staticmesh.new_id);
			}
			break;
		}

		case EditorCommandType::SET_MATERIAL_COMPONENT: {
			EditorCommand_SetMaterialComponent set_material = command.cmd.set_material;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_render_material(entity_manager, set_material.entity, set_material.old_id);
			}
			else {
				set_render_material(entity_manager, set_material.entity, set_material.new_id);
			}
			break;
		}
		case EditorCommandType::SET_LIGHT_COMPONENT: {
			EditorCommand_SetLightComponent set_light_data = command.cmd.set_light;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_light(entity_manager, set_light_data.entity, set_light_data.old_light);
			}
			else {
				set_light(entity_manager, set_light_data.entity, set_light_data.new_light);
			}
			break;
		}
		case EditorCommandType::SET_CAMERA_COMPONENT: {
			EditorCommand_SetCameraComponent set_camera_data = command.cmd.set_camera;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_camera(entity_manager, set_camera_data.entity, set_camera_data.old_camera);
			}
			else {
				set_camera(entity_manager, set_camera_data.entity, set_camera_data.new_camera);
			}
			break;
		}

		default: {
			// Unknown command was sent
			assert(false && "Unknown Editor command. Did you forget to implement an editor command?");
		}
	}



}



static void process_editor_command_buffer(EditorInterface* editor) {
	EditorCommandBuffer* cmd_buffer = &editor->cmd_buffer;
	const size_t command_count = cmd_buffer->command_count;
	for (size_t i = 0; i < command_count; i++) {
		const EditorCommand& command = cmd_buffer->commands[i];
		perform_command(editor, command, false);
	}

	
	cmd_buffer->command_count = 0;
}


static void cmd_editor_group_begin(EditorInterface* editor) {
	size_t group_count = editor->cmd_buffer.command_group_stack_count;


	EditorCommand command;
	command.type = EditorCommandType::COMMAND_GROUP_START;
	command.cmd.group.stack_level = group_count;
	push_editor_command(editor, command);

	assert(editor->cmd_buffer.group_stack[group_count].in_group_transaction == false && "Did you forget to call cmd_editor_group_end?");

	

	editor->cmd_buffer.group_stack[group_count].in_group_transaction = true;
	editor->cmd_buffer.command_group_stack_count++;

	//editor->cmd_buffer.current_group.in_group_transaction = true;
	//editor->cmd_buffer.current_group.command_count = 0;

}
static void cmd_editor_group_end(EditorInterface* editor) {
	
	size_t group_count = editor->cmd_buffer.command_group_stack_count;

	// Crash if this is not a group transaction
	assert(editor->cmd_buffer.group_stack[group_count - 1].in_group_transaction == true && "Did you forget to call cmd_editor_group_begin?");


	EditorCommand command;
	command.type = EditorCommandType::COMMAND_GROUP_END;
	command.cmd.group = editor->cmd_buffer.group_stack[group_count - 1];
	command.cmd.group.stack_level = group_count - 1;
	push_editor_command(editor, command);

	
	
	// Reset command transaction
	// Even though this will be guaranteed to happen when you call group begin
	// this prevents weird lingering issues when group begin is not called
	editor->cmd_buffer.group_stack[group_count - 1].in_group_transaction = false;
	editor->cmd_buffer.command_group_stack_count--;
	//editor->cmd_buffer.current_group.in_group_transaction = false;

	
}

static void cmd_editor_force_no_merge(EditorInterface* editor) {
	EditorCommand command;
	command.type = EditorCommandType::COMMAND_FORCE_NO_MERGE;
	push_editor_command(editor, command);
}

static void cmd_editor_create_emtpy_entity(EditorInterface* editor) {

	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Entity");

	ComponentType comp_flags = ComponentType::None;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);

}

static void cmd_editor_create_plane(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Plane");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->plane_mesh.mesh;
	command.cmd.new_entity.material_id = editor->test_mat.material;

	push_editor_command(editor, command);

}

static void cmd_editor_create_sphere(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Sphere");



	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->sphere_mesh.mesh;
	command.cmd.new_entity.material_id = editor->test_mat.material;


	push_editor_command(editor, command);

}


static void cmd_editor_create_cube(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Cube");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->cube_mesh.mesh;
	command.cmd.new_entity.material_id = editor->test_mat.material;


	push_editor_command(editor, command);

}

static void cmd_editor_create_light(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Light");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::Light;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);

}


static void cmd_editor_create_camera(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Camera");
	ComponentType comp_flags = ComponentType::None;

	comp_flags = comp_flags | ComponentType::Camera;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);
}


static void cmd_editor_select_entity(EditorInterface* editor, Entity entity, bool selected) {
	EditorCommand command;
	command.type = EditorCommandType::SELECT_ENTITY;
	command.cmd.select_entity.entity_to_select = entity;
	command.cmd.select_entity.selected = selected;
	

	push_editor_command(editor, command);
}

static void cmd_editor_deselect_all_entitys(EditorInterface* editor) {
	EntityManager* em = editor->api.entity_manager;

	if (editor->entity_selected_count > 0) {
		cmd_editor_group_begin(editor);
		for (int i = 0; i < em->entity_count; i++) {
			Entity e = em->entity_list[i];
			// Only deselect the ones that are selected
			if (is_entity_selected(editor, e)) {
				cmd_editor_select_entity(editor, e, false);
			}
		}
		cmd_editor_group_end(editor);
	}
	

	
	
}


static void cmd_edtior_set_transform_component(EditorInterface* editor, Entity e, Vec3f old_pos, Quat old_rot, Vec3f old_scale, Vec3f pos, Quat rot, Vec3f scale, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_TRANSFORM_COMPONENT;
	command.cmd.set_transform.entity = e;


	command.cmd.set_transform.position = pos;
	command.cmd.set_transform.old_position = old_pos;

	command.cmd.set_transform.scale = scale;
	command.cmd.set_transform.old_scale = old_scale;

	command.cmd.set_transform.rotation = rot;
	command.cmd.set_transform.old_rotation = old_rot;
	
	// We only merge commands that are on the undo stack
	// This is usually used for when we starts a command on one frame, but need to end a command on another frame
	// For example if you start dragging a transform one frame 1, it might not also end on frame 1. So we need to merge multiple frames for one command
	// Therefore a simple way to solve this problem is to merge two commands
	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_TRANSFORM_COMPONENT) {
				top_command->cmd.set_transform.position = pos;
				top_command->cmd.set_transform.scale = scale;
				top_command->cmd.set_transform.rotation = rot;
				return;
			}
		}
	}
	push_editor_command(editor, command);
	
}


static void cmd_editor_set_staticmesh_component(EditorInterface* editor, Entity e, StaticMeshID old_id, StaticMeshID new_id) {
	EditorCommand command;
	command.type = EditorCommandType::SET_STATICMESH_COMPONENT;
	command.cmd.set_staticmesh.entity = e;
	command.cmd.set_staticmesh.old_id = old_id;
	command.cmd.set_staticmesh.new_id = new_id;

	push_editor_command(editor, command);
	
}


static void cmd_editor_set_material_component(EditorInterface* editor, Entity e, MaterialID old_id, MaterialID new_id) {
	EditorCommand command;
	command.type = EditorCommandType::SET_MATERIAL_COMPONENT;
	command.cmd.set_material.entity = e;
	command.cmd.set_material.old_id = old_id;
	command.cmd.set_material.new_id = new_id;

	push_editor_command(editor, command);
}

static void cmd_editor_set_light_component(EditorInterface* editor, Entity e, Light old_light, Light new_light, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_LIGHT_COMPONENT;
	command.cmd.set_light.entity = e;
	command.cmd.set_light.old_light = old_light;
	command.cmd.set_light.new_light = new_light;

	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_LIGHT_COMPONENT) {
				top_command->cmd.set_light.new_light = new_light;
				return;
			}
		}
	}

	push_editor_command(editor, command);
}

static void cmd_editor_set_camera_component(EditorInterface* editor, Entity e, Camera old_camera, Camera new_camera, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_CAMERA_COMPONENT;

	
	
	command.cmd.set_camera.entity = e;
	command.cmd.set_camera.old_camera = old_camera;
	command.cmd.set_camera.new_camera = new_camera;

	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_CAMERA_COMPONENT) {
				top_command->cmd.set_camera.new_camera = new_camera;
				return;
			}
		}
	}

	push_editor_command(editor, command);
}