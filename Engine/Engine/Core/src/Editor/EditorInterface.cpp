#pragma once

#include "Editor/EditorInterface.h"
#include "Logger.h"


#include "SDL_syswm.h"

#include "../../imgui/imgui.h"


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





	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e1 = create_entity(entity_manager, "Entity 2");
	Entity e2 = create_entity(entity_manager, "Entity 3");
	Entity e3 = create_entity(entity_manager, "Entity 4");
	Entity e4 = create_entity(entity_manager, "Entity 5");
	Entity e5 = create_entity(entity_manager, "Entity 6");
	Entity e6 = create_entity(entity_manager, "Entity 7");

	attach_child_entity(entity_manager, e1, e2);
	attach_child_entity(entity_manager, e1, e6);
	attach_child_entity(entity_manager, e2, e3);
	attach_child_entity(entity_manager, e1, e4);

	

	map_put(&editor->entity_selected, editor->editor_camera.id, false);
	map_put(&editor->entity_selected, e1.id, false);
	map_put(&editor->entity_selected, e2.id, false);
	map_put(&editor->entity_selected, e3.id, false);
	map_put(&editor->entity_selected, e4.id, false);
	map_put(&editor->entity_selected, e5.id, false);
	map_put(&editor->entity_selected, e6.id, false);

	

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


static void draw_entity_tree(EditorInterface* editor, Entity e) {

	MapResult<bool> result = map_get(&editor->entity_selected, e.id);
	
	auto em = editor->api.entity_manager;
	// We need to recursivly draw the children
	Entity child = first_child(em, e);
	
	
	String name = get_name(&em->transform_manager, e);
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
		
		ImGui::Unindent();
	} else {
		
		// This is a node that has children
		
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((result.value) ? ImGuiTreeNodeFlags_Selected : 0);


		bool node_open = ImGui::TreeNodeEx(name.buffer, node_flags);


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
				if (ImGui::MenuItem("Create Emtpty Entity", "Ctrl+Alt+N")) {}
				if (ImGui::MenuItem("Create Camera")) {}
				if (ImGui::MenuItem("Create Light")) {}
				if (ImGui::MenuItem("Create Plane")) {}
				if (ImGui::MenuItem("Create Cube")) {}
				if (ImGui::MenuItem("Create Sphere")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Assets")) {

				if (ImGui::MenuItem("Import")) {}
				if (ImGui::MenuItem("Create Material")) {}
				if (ImGui::MenuItem("Create Folder")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window")) {

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::Begin("ENGINE TIMERS");
			
		ImGui::Checkbox("Cap framerate", &editor->api.game_loop->cap_framerate);
		if (editor->api.game_loop->cap_framerate) {
			ImGui::SliderInt("Target Framerate", &editor->api.game_loop->target_fps, 0, 240);
		}
		editor->fps_history[editor->fps_history_index] = timer->fps;
		editor->fps_history_index = ((editor->fps_history_index + 1 )% FPS_HISTORY_COUNT);
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

		ImGui::End();

		


		ImGui::Begin("Entity Scene Tree");
		ImGui::Text("Total Entitys Created %d", entity_manager->entitys_created);
		ImGui::Text("Current Entity Count %d", entity_manager->entity_count);
		ImGui::Separator();


		static char str0[128] = "";
		ImGui::Text("Filter Entitys");
		ImGui::InputText("", str0, IM_ARRAYSIZE(str0));
		ImGui::Separator();
			


			
		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
			
			String name = get_name(&entity_manager->transform_manager, e);
			Entity parent_entity = parent(entity_manager, e);

			if (parent_entity.id == NO_ENTITY_ID) {	
				draw_entity_tree(editor, e);
			}
			
		}

		ImGui::End();

		


		
		
		ImGui::Begin("Entity Components");
		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
			MapResult<bool> result = map_get(&editor->entity_selected, e.id);
			if (!result.found) continue;
			if (result.value) {
				ImGui::PushID(e.id);
				String e_name = get_name(&entity_manager->transform_manager, e);

				
			
				ImGui::InputText("", e_name.buffer, e_name.length);
				ImGui::SameLine();
				bool enabled = 0;
				if (ImGui::Checkbox("Enabled", &enabled)) {
					printf("enabled");
				}

				if (has_component(entity_manager, e, ComponentType::Transform)) {
					draw_component_transform(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::Camera)) {
					draw_component_camera(editor, e);
				}
				ImGui::PopID();


				
				
				ImGui::Separator();
				ImGui::Spacing();
				
			}

			

			

		}
		
		ImGui::End();
		


		

		ImGui::Begin("Log");


		
		LogList logs = g_get_loglist();

		static char filter_buffer[128];
		ImGui::InputTextWithHint("", "Filter", filter_buffer, 128);
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
					ImGui::Text(log_type_str);
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
		

		
		
		
		
		ImGui::End();

		

		



		
	
	} // END OF SHOW EDITOR

	

	
	


	

	


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

		LOG_WARN("EDITOR", "scroll test");
	}





	int x = input->mouse.pos.x;
	int y = input->mouse.pos.y;

	int gx = input->mouse.global_pos.x;
	int gy = input->mouse.global_pos.y;

	int sx = window->size.x;
	int sy = window->size.y;

	Vec2i delta_pos = input->mouse.delta_pos;



	//debug_print("mouse pos %d,%d\t, delta pos %d,%d\tglobal mos pos %d,%d, window size %d,%d\n", x, y, delta_pos.x, delta_pos.y, gx, gy, sx, sy);


	
	
	/*if (is_mouse_pressed(input, MouseButton::Left)) {
		create_entity(entity_manager, "Entity test");
	}

	if (is_mouse_pressed(input, MouseButton::Middle)) {
		u64 last_entity = entity_manager->entity_count;
		auto e = entity_manager->entity_list[last_entity - 1];
		destroy_entity(entity_manager, e);
	}*/

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

		// Prevent rotation jump, when the delta between the last time the right mouse was down and now


		// TODO: this will be exposed to the user, we still need to implement proper control handling in engine
		float sensitivity = 0.25f;
		Quat old_cam_rot = get_rotation(entity_manager, editor->editor_camera);
		Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -rel_pos.x * sensitivity) * old_cam_rot;
		new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -rel_pos.y * sensitivity);

		set_rotation(entity_manager, editor->editor_camera, new_cam_rot);

	} else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
		

		

	


}

static void draw_component_transform(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	if (ImGui::CollapsingHeader("Transform")) {
		Vec3f pos = get_position(entity_manager, e);
		Quat quat = get_rotation(entity_manager, e);
		Vec3f euler = quat_to_euler(quat);

		Vec3f scale = get_scale(entity_manager, e);

		if (ImGui::DragFloat3("Position", pos.data, 1.0f)) {
			set_position(entity_manager, e, pos);
		}

		if (ImGui::DragFloat3("Scale", scale.data, 1.0f)) {
			set_scale(entity_manager, e, scale);
		}
		if (ImGui::DragFloat3("Rotation", euler.data, 1.0f)) {
			quat = euler_to_quat(euler);
			set_rotation(entity_manager, e, quat);
		}
	}
	
}


static void draw_component_camera(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;

	
	if (ImGui::CollapsingHeader("Camera")) {
		Camera* cam = get_camera(entity_manager, e);
		//ImGui::DragFloat("Aspect Ratio", &cam->aspect_ratio);

		int projection = (int)cam->projection;
		if (ImGui::Combo("Projection", &projection, "Perspective\0Orthographic\0\0")) {
			cam->projection = (CameraProjection)projection;
		}

		ImGui::SliderFloat("Field Of View", &cam->fov, 30.0f, 120.0f);
		ImGui::DragFloat("Near", &cam->near_clip, 1.0f, 0.0f, FLT_MAX);
		ImGui::DragFloat("Far", &cam->far_clip, 1.0f, 0.0f, FLT_MAX);


		if (cam->projection == CameraProjection::Orthographic) {

			ImGui::Separator();
			ImGui::Text("Orthographic Settings");
			ImGui::DragFloat("Left", &cam->left, 1.0f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Right", &cam->right, 1.0f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Top", &cam->top, 1.0f, 0.0f, FLT_MAX);
			ImGui::DragFloat("Bottom", &cam->bottom, 1.0f, 0.0f, FLT_MAX);
		}
	}
}