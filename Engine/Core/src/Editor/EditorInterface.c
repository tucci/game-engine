#pragma once

#include "Editor/EditorInterface.h"

#include "SDL_syswm.h"


bool init_editor_interface(EditorInterface* editor, EngineAPI api) {
	editor->api = api;

	arena_init(&editor->arena);
	size_t mem_size = EDITOR_MEMORY;
	void* mem_block = arena_alloc(&editor->arena, mem_size);
	mem_size = editor->arena.end - cast(char*) mem_block;
	stack_alloc_init(&editor->stack, mem_block, mem_size);

	editor->editor_camera = create_entity(api.entity_manager);
	add_component(api.entity_manager, editor->editor_camera , ComponentType_Transform);
	add_component(api.entity_manager, editor->editor_camera , ComponentType_Camera);

	set_camera_params(api.entity_manager, editor->editor_camera, 0.0001f, 100.0f, 90.0f, api.window->size.x / cast(float) api.window->size.y);
	set_position(api.entity_manager, editor->editor_camera, Vec3f(0, 0, 0));

	
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);

	

	connect_editor_socket(editor);

	return true;
}

void destroy_editor_interface(EditorInterface* editor) {
	disconnect_editor_socket(editor);

	arena_free(&editor->arena);
}

void editor_update(EditorInterface* editor) {

	Input* input = editor->api.input;
	GameTimer* timer = editor->api.game_loop;
	EntityManager* entity_manager = editor->api.entity_manager;

	Renderer* renderer = editor->api.renderer;
	Window* window = editor->api.window;
	
	

	float delta_time = timer->delta_time;
	Camera* camera = get_camera(entity_manager, editor->editor_camera);




	Vec2i scroll = get_scroll_delta(input);


	// Capture scolling to move camera forward and back
	if (scroll.y != 0) {
		Vec3f new_cam_direction = (delta_time * -forward(entity_manager, editor->editor_camera));

		// TODO: make this configurable
		float scroll_scale = 10.0f;

		float cam_move_scale = scroll_scale * scroll.y;
		Vec3f cam_pos = position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));
	}





	int x = input->mouse.pos.x;
	int y = input->mouse.pos.y;

	int gx = input->mouse.global_pos.x;
	int gy = input->mouse.global_pos.y;

	int sx = window->size.x;
	int sy = window->size.y;

	Vec2i delta_pos = input->mouse.delta_pos;



	//debug_print("mouse pos %d,%d\t, delta pos %d,%d\tglobal mos pos %d,%d, window size %d,%d\n", x, y, delta_pos.x, delta_pos.y, gx, gy, sx, sy);




	// Only apply editor movement if right mouse button is clicked
	if (is_mouse_down(input, MouseButton_Right)) {
		//SDL_SetWindowGrab(window->sdl_window, SDL_TRUE);
		SDL_ShowCursor(SDL_DISABLE);

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
		Vec3f cam_pos = position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));

		// Prevent rotation jump, when the delta between the last time the right mouse was down and now
		if (editor->was_last_frame_using_right_click) {

			// TODO: this will be exposed to the user, we still need to implement proper control handling in engine
			float sensitivity = 0.25f;
			Quat old_cam_rot = rotation(entity_manager, editor->editor_camera);
			Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -rel_pos.x * sensitivity) * old_cam_rot;
			new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -rel_pos.y * sensitivity);

			set_rotation(entity_manager, editor->editor_camera, new_cam_rot);
		}

		// Keep the mouse in the center of the window.
		SDL_WarpMouseInWindow(window->sdl_window, window->size.x / 2, window->size.y / 2);
		editor->was_last_frame_using_right_click = true;

	} else {
		//SDL_SetWindowGrab(window->sdl_window, SDL_FALSE);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
		//SDL_CaptureMouse(SDL_FALSE);

		editor->was_last_frame_using_right_click = false;
	}



}

void connect_editor_socket(EditorInterface* editor) {
	int result = sock_init();
	if (result != 0) {
		// error
		return;
	}
	
	editor->socket = sock_create_handle();
	sock_bind(editor->socket);
	sock_listen(editor->socket);
	sock_accept(editor->socket);

	
	Window* window = editor->api.window;

	

	

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window->sdl_window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	debug_print("%d\n", (u32)hwnd);

	EditorCommand command = cmd_send_window((u32)hwnd);
	send_command_to_editor(editor, command);


}

void disconnect_editor_socket(EditorInterface* editor) {
	sock_close(editor->socket);
	sock_shutdown(editor->socket);
	sock_free_handle(editor->socket);
}


// Includes the payload size and non payload size(ex command type)
static size_t get_size_for_command(EditorCommandType type) {
	// Add the base size starting the with the command type
	size_t size = sizeof(EditorCommandType);
	switch (type) {
		case EditorCommand_NONE: {
			size += 0;
			break;
		}
		case EditorCommand_ESTABLISH_CONNECTION: {
			size += sizeof(u32);
			break;
		}
		
	}

	return size;
}



void send_command_to_editor(EditorInterface* editor, EditorCommand command) {

	
		

	EditorCommandHeader header;
	header.endianness = 1;
	header.version = EDITOR_COMMAND_VESRSION;
	header.message_size = get_size_for_command(command.type);
	
	
	// NOTE: this assumes the size of header is already packed, padded, and aligned
	size_t header_size = sizeof(header);
	size_t command_size = header.message_size;

	size_t buf_size = header_size + command_size;





	char* buf = (char*)stack_alloc(&editor->stack, buf_size, 1);

	
	memcpy(buf, &header, header_size);
	memcpy(buf + header_size, &command, command_size);
	//snprintf(buf, 256, "%d", (u32)hwnd);s

	sock_send(editor->socket, buf, buf_size, 0);

	stack_pop(&editor->stack);


}