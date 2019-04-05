#pragma once

#include "Editor/EditorInterface.h"
#include "Logger.h"


#include "SDL_syswm.h"




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

	

	editor->editor_camera = create_entity(api.entity_manager, "Editor Camera");
	//add_component(api.entity_manager, editor->editor_camera , ComponentType::Transform);
	add_component(api.entity_manager, editor->editor_camera , ComponentType::Camera);

	set_camera_params(api.entity_manager, editor->editor_camera, 0.0001f, 100.0f, 90.0f, api.window->size.x / cast(float) api.window->size.y);
	set_position(api.entity_manager, editor->editor_camera, Vec3f(0, 0, 0));

	
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);






	



	//AssetID tcolor = import_texture(editor->api.asset_manager, IString("Assets/textures/paint_cement/wornpaintedcement-albedo.png"), true);
	//AssetID tnormal = import_texture(editor->api.asset_manager, IString("Assets/textures/paint_cement/wornpaintedcement-normal.png"), true);
	//AssetID tmetallic = import_texture(editor->api.asset_manager, IString("Assets/textures/paint_cement/wornpaintedcement-metalness.png"), true);
	//AssetID troughness = import_texture(editor->api.asset_manager, IString("Assets/textures/paint_cement/wornpaintedcement-roughness.png"), true);
	//AssetID tao = import_texture(editor->api.asset_manager, IString("Assets/textures/paint_cement/wornpaintedcement-ao.png"), true);
	//
	//Material test_mat;
	//init_material_defaults(&test_mat);
	//
	//test_mat.albedo = tcolor.texture;
	//test_mat.normal = tnormal.texture;
	//test_mat.metal = tmetallic.texture;
	//test_mat.roughness = troughness.texture;
	//test_mat.ao = tao.texture;
	//
	//AssetID mat_id = create_material_asset(editor->api.asset_manager, "Assets/textures/paint_cement/", "paint_cement_mat", &test_mat);

	//AssetID mat_id = load_asset_by_name(editor->api.asset_manager, "Assets/textures/bamboo-wood/bamboo-wood_mat.easset");
	//AssetID mat_id = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat_mat.easset");
	//AssetID mat_id = load_asset_by_name(editor->api.asset_manager, "Assets/textures/rust_iron/rust_mat_mat.easset");
	editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/paint_cement/paint_cement_mat_mat.easset");
	
	
	
	



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
	
	set_static_mesh(entity_manager, editor->test_mesh, editor->api.asset_manager->cube_mesh.mesh);


	add_component(entity_manager, e3, ComponentType::StaticMesh);
	add_component(entity_manager, e3, ComponentType::Render);
	set_render_material(entity_manager, e3, editor->test_mat.material);
	set_static_mesh(entity_manager, e3, editor->api.asset_manager->sphere_mesh.mesh);


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

	

	return true;
}

void destroy_editor_interface(EditorInterface* editor) {
	map_destroy(&editor->entity_selected);
	arena_free(&editor->arena);
}


static void clear_entity_selection(EditorInterface* editor) {
	
	EntityManager* em = editor->api.entity_manager;
	for (int i = 0; i < em->entity_count; i++) {
		Entity e = em->entity_list[i];
		MapResult<bool> result = map_get(&editor->entity_selected, e.id);
		if (result.found) {
			map_put(&editor->entity_selected, e.id, false);
		}
	}

}



void editor_update(EditorInterface* editor) {

	
	Input* input = editor->api.input;
	GameTimer* timer = editor->api.game_loop;
	EntityManager* entity_manager = editor->api.entity_manager;

	Renderer* renderer = editor->api.renderer;
	Window* window = editor->api.window;

	
	

	

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

		
		
	
	} // END OF SHOW EDITOR

	

	
	


	

	

	if (is_mouse_pressed(input, MouseButton::Left)) {
		Vec2i mouse_pos = get_mouse_pos(input);

		// Do a raycast to check which object is pressed
		LOG_INFO("MOUSE", "Mouse pos %d, %d\n", mouse_pos.x, mouse_pos.y);
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
				editor_create_empty_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				editor_create_sphere(editor);
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
	
	
	//remove component
	
	if (ImGui::CollapsingHeader("Transform")) {
		Vec3f pos = get_position(entity_manager, e);
		Quat quat = get_rotation(entity_manager, e);
		Vec3f euler = quat_to_euler(quat);

		Vec3f scale = get_scale(entity_manager, e);

		
		ImGui::PushID("pos_default");
		if (ImGui::SmallButton("z")) {
			set_position(entity_manager, e, Vec3f(0,0,0));
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Position", pos.data, 1.0f)) {
			set_position(entity_manager, e, pos);
		}
		ImGui::PopID();
		
		ImGui::PushID("rot_default");

		if (ImGui::SmallButton("z")) {
			set_rotation(entity_manager, e, Quat());
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Rotation", euler.data, 1.0f)) {
			quat = euler_to_quat(euler);
			set_rotation(entity_manager, e, quat);
		}
		ImGui::PopID();


		ImGui::PushID("scale_default");
		
		if (ImGui::SmallButton("z")) {
			set_scale(entity_manager, e, Vec3f(1, 1, 1));
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Scale", scale.data, 1.0f)) {
			set_scale(entity_manager, e, scale);
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
			
		
		Camera* cam = get_camera(entity_manager, e);
		//ImGui::DragFloat("Aspect Ratio", &cam->aspect_ratio);

		int projection = (int)cam->projection;
		if (ImGui::Combo("Projection", &projection, "Perspective\0Orthographic\0\0")) {
			cam->projection = (CameraProjection)projection;
		}

		ImGui::PushID("camera_fov_default");
		if (ImGui::SmallButton("z")) {
			cam->fov = 90.0f;
		}
		ImGui::SameLine();
		ImGui::SliderFloat("Field Of View", &cam->fov, 30.0f, 120.0f);
		ImGui::PopID();

		ImGui::PushID("camera_near_default");
		if (ImGui::SmallButton("z")) {
			cam->near_clip = 0.01f;
		}
		ImGui::SameLine();
		ImGui::DragFloat("Near", &cam->near_clip, 1.0f, 0.0f, FLT_MAX);
		ImGui::PopID();

		ImGui::PushID("camera_far_default");
		if (ImGui::SmallButton("z")) {
			cam->far_clip = 100.0f;
		}
		ImGui::SameLine();

		ImGui::DragFloat("Far", &cam->far_clip, 1.0f, 0.0f, FLT_MAX);
		ImGui::PopID();


		if (cam->projection == CameraProjection::Orthographic) {

			ImGui::Separator();
			ImGui::Text("Orthographic Settings");

			ImGui::PushID("camera_left_default");
			if (ImGui::SmallButton("z")) {
				cam->left = 0.0f;
			}
			ImGui::SameLine();
			ImGui::DragFloat("Left", &cam->left, 1.0f, 0.0f, FLT_MAX);
			ImGui::PopID();

			ImGui::PushID("camera_right_default");
			if (ImGui::SmallButton("z")) {
				cam->right = 100.0f;
			}
			ImGui::SameLine();
			ImGui::DragFloat("Right", &cam->right, 1.0f, 0.0f, FLT_MAX);
			ImGui::PopID();

			ImGui::PushID("camera_top_default");
			if (ImGui::SmallButton("z")) {
				cam->top = 0.0f;
			}
			ImGui::SameLine();
			ImGui::DragFloat("Top", &cam->top, 1.0f, 0.0f, FLT_MAX);
			ImGui::PopID();

			ImGui::PushID("camera_bottom_default");
			if (ImGui::SmallButton("z")) {
				cam->bottom = 100.0f;
			}
			ImGui::SameLine();

			ImGui::DragFloat("Bottom", &cam->bottom, 1.0f, 0.0f, FLT_MAX);
			ImGui::PopID();
		}
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
			
		
		
		

		Light light =  get_light(entity_manager, e);
		int light_type = (int)light.type;
		if (ImGui::Combo("Type", &light_type, "None\0Directional Light\0Point Light\0\0")) {
			light.type = (LightType)light_type;
		}

		switch (light.type) {
			case LightType::DirectionalLight: {


				ImGui::PushID("dir_direction_default");
				if (ImGui::SmallButton("z")) {
					light.dir_light.direction = Vec3f(0, 0, 0);
				}
				ImGui::SameLine();
				ImGui::DragFloat3("Direction", light.dir_light.direction.data);
				ImGui::PopID();

				ImGui::PushID("dir_color_default");
				if (ImGui::SmallButton("z")) {
					light.dir_light.color = Vec3f(1, 1, 1);
				}
				ImGui::SameLine();
				ImGui::ColorEdit3("Color", light.dir_light.color.data);
				ImGui::PopID();



				ImGui::PushID("dir_intensity_default");
				if (ImGui::SmallButton("z")) {
					light.dir_light.intensity = 0;
				}
				ImGui::SameLine();

				ImGui::DragFloat("Intensity", &light.dir_light.intensity, 1.0f, 0.0f, 100000.0f);
				ImGui::PopID();

				

				break;
			}
			case LightType::PointLight: {
				ImGui::PushID("point_color_default");
				if (ImGui::SmallButton("z")) {
					light.point_light.color = Vec3f(1,1,1);
				}
				ImGui::SameLine();
				ImGui::ColorEdit3("Color", light.point_light.color.data);
				ImGui::PopID();
				break;
			}
		}

		set_light(entity_manager, e, light);
		
		
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
		
		ImGui::Text("Mesh id %lld", mesh_id.id);
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
		ImGui::Text("Material id %lld", mat_id.id);
		
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

	if (ImGui::Begin("Render Components")) {
		RenderManager* rm = &entity_manager->render_manager;
		ImGui::Text("Capacity %d", rm->capacity);
		ImGui::Text("Enabled %d", rm->enabled_count);

		for (u64 i = 0; i < rm->enabled_count; i++) {
			ImGui::Text("%d ", rm->entitys[i].id);
			ImGui::SameLine();
		}
		ImGui::NewLine();
		for (u64 i = 0; i < rm->capacity; i++) {
			ImGui::Text("%d ", rm->entitys[i].id);
			ImGui::SameLine();
		}

		ImGui::NewLine();
		size_t map_size = rm->id_map.size;
		// Go over our track map, and look for filename
		for (int i = 0; i < map_size; i++) {
			CompactMapItem<u64> item = rm->id_map.map[i];
			
			// Check if this is a valid track data
			if (item.key!= 0 && item.key != TOMBSTONE) {
				ImGui::Text("%llu -> %llu", item.key, item.value);
			}
		}

	}
	ImGui::End();
		

}

static void draw_entity_tree(EditorInterface* editor, Entity e) {

	MapResult<bool> result = map_get(&editor->entity_selected, e.id);

	auto em = editor->api.entity_manager;
	// We need to recursivly draw the children
	Entity child = first_child(em, e);


	String name = get_name(&em->meta_manager, e);
	
	ImGui::PushID(e.id);
	if (child.id == NO_ENTITY_ID) {
		// This is a leaf node
		ImGui::Indent();
		
		if (ImGui::Selectable(name.buffer, result.value)) {
			if (!ImGui::GetIO().KeyCtrl) {
				// Clear selection when CTRL is not held
				clear_entity_selection(editor);
			}
			map_put(&editor->entity_selected, e.id, !result.value);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}
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
				destroy_entity(em, e);
			}


			ImGui::EndPopup();
		}
		

		ImGui::Unindent();
	} else {


		// This is a node that has children

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((result.value) ? ImGuiTreeNodeFlags_Selected : 0);


		bool node_open = ImGui::TreeNodeEx(name.buffer, node_flags);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}
			


		if (ImGui::IsItemClicked()) {
			if (!ImGui::GetIO().KeyCtrl) {
				// Clear selection when CTRL is not held
				clear_entity_selection(editor);
			}
			map_put(&editor->entity_selected, e.id, !result.value);
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
		
		
		
		ImGui::Text("Total Entitys Created %d", entity_manager->entitys_created);
		ImGui::Text("Current Entity Count %d", entity_manager->entity_count);
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

			if (parent_entity.id == NO_ENTITY_ID) {
				draw_entity_tree(editor, e);
			}
			
		}

		ImGui::BeginChild("scene_internal");

		if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(0, false)) {
			clear_entity_selection(editor);
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
				editor_create_empty_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				editor_create_sphere(editor);
			}
			ImGui::EndPopup();
		}

		ImGui::EndChild();
		

	}
	ImGui::End();

	if (ImGui::Begin("Transform Components")) {
		TransformManager* rm = &entity_manager->transform_manager;
		ImGui::Text("Capacity %d", rm->total_count);
		ImGui::Text("Enabled %d", rm->enabled_count);

		//for (u64 i = 0; i < rm->enabled_count; i++) {
		//	ImGui::Text("%d ", rm->entitys[i].id);
		//	ImGui::SameLine();
		//}
		//ImGui::NewLine();
		//for (u64 i = 0; i < rm->capacity; i++) {
		//	ImGui::Text("%d ", rm->entitys[i].id);
		//	ImGui::SameLine();
		//}
		//
		//ImGui::NewLine();
		//size_t map_size = rm->id_map.size;
		//// Go over our track map, and look for filename
		//for (int i = 0; i < map_size; i++) {
		//	CompactMapItem<u64> item = rm->id_map.map[i];
		//
		//	// Check if this is a valid track data
		//	if (item.key != 0 && item.key != TOMBSTONE) {
		//		ImGui::Text("%llu -> %llu", item.key, item.value);
		//	}
		//}

		ImGui::Text("Parents");
		ImGui::SameLine();
		for (u64 i = 0; i < rm->enabled_count; i++) {
			ImGui::Text("%llu ", rm->parent[i]);
			ImGui::SameLine();
		}
		ImGui::NewLine();

		ImGui::Text("1childs");
		ImGui::SameLine();
		for (u64 i = 0; i < rm->enabled_count; i++) {
			ImGui::Text("%llu ", rm->first_child[i]);
			ImGui::SameLine();
		}
		ImGui::NewLine();
		ImGui::Text("Nxtsibs");
		ImGui::SameLine();
		for (u64 i = 0; i < rm->enabled_count; i++) {
			ImGui::Text("%llu ", rm->next_sibling[i]);
			ImGui::SameLine();
		}
		ImGui::NewLine();
		ImGui::Text("Prvsibs");
		ImGui::SameLine();
		for (u64 i = 0; i < rm->enabled_count; i++) {
			ImGui::Text("%llu ", rm->prev_sibling[i]);
			ImGui::SameLine();
		}

	}
	ImGui::End();
	
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
				ImGui::Text(track_item.value.filename);
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
	ImGui::End();
	}

	if (ImGui::Begin("Game")) {
	ImGui::End();
	}

	if (ImGui::Begin("Multi Windows")) {
	ImGui::End();
	}

}

static void editor_create_empty_entity(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e = create_entity(entity_manager, "Entity");

	map_put(&editor->entity_selected, e.id, false);
}

static void editor_create_plane(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	Entity e = create_entity(entity_manager, "Plane");
	
	map_put(&editor->entity_selected, e.id, false);

	add_component(entity_manager, e, ComponentType::StaticMesh);
	add_component(entity_manager, e, ComponentType::Render);
	set_render_material(entity_manager, e, editor->test_mat.material);
	set_static_mesh(entity_manager, e, editor->api.asset_manager->plane_mesh.mesh);

}

static void editor_create_sphere(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e = create_entity(entity_manager, "Sphere");

	map_put(&editor->entity_selected, e.id, false);

	add_component(entity_manager, e, ComponentType::StaticMesh);
	add_component(entity_manager, e, ComponentType::Render);
	set_render_material(entity_manager, e, editor->test_mat.material);
	set_static_mesh(entity_manager, e, editor->api.asset_manager->sphere_mesh.mesh);

}


static void editor_create_cube(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e = create_entity(entity_manager, "Cube");

	map_put(&editor->entity_selected, e.id, false);

	add_component(entity_manager, e, ComponentType::StaticMesh);
	add_component(entity_manager, e, ComponentType::Render);
	set_render_material(entity_manager, e, editor->test_mat.material);
	set_static_mesh(entity_manager, e, editor->api.asset_manager->cube_mesh.mesh);

}

static void editor_create_light(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e = create_entity(entity_manager, "Light");

	map_put(&editor->entity_selected, e.id, false);
	add_component(entity_manager, e, ComponentType::Light);
}


static void editor_create_camera(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e = create_entity(entity_manager, "Camera");

	map_put(&editor->entity_selected, e.id, false);
	add_component(entity_manager, e, ComponentType::Camera);
}