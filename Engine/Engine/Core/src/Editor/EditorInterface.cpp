#pragma once

#include "Editor/EditorInterface.h"
#include "Logger.h"

#include "SDL_syswm.h"


bool init_editor_interface(EditorInterface* editor, EngineAPI api) {
	editor->api = api;
	editor->connection_status = EditorConnectionStatus::None;
	

	arena_init(&editor->arena);
	size_t mem_size = EDITOR_MEMORY;
	void* mem_block = arena_alloc(&editor->arena, mem_size);
	mem_size = editor->arena.end - cast(char*) mem_block;
	stack_alloc_init(&editor->stack, mem_block, mem_size);



	// Init networking interface
	editor->ecq.packet_bytes_read = 0;
	editor->ecq.command_expected_size = 0;
	editor->ecq.packet_buffer = cast(char*)(stack_alloc(&editor->stack, MEGABYTES(10), 1));
	

	editor->editor_camera = create_entity(api.entity_manager, "Editor Camera");
	//add_component(api.entity_manager, editor->editor_camera , ComponentType::Transform);
	add_component(api.entity_manager, editor->editor_camera , ComponentType::Camera);

	set_camera_params(api.entity_manager, editor->editor_camera, 0.0001f, 100.0f, 90.0f, api.window->size.x / cast(float) api.window->size.y);
	set_position(api.entity_manager, editor->editor_camera, Vec3f(0, 0, 0));

	
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "Assets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");
	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);

	

	connect_editor_socket(editor);



	EntityManager* entity_manager = editor->api.entity_manager;

	Entity e1 = create_entity(entity_manager, "Entity 1");
	Entity e2 = create_entity(entity_manager, "Entity 2");

	Entity e3 = create_entity(entity_manager, "Entity 3");

	Entity e4 = create_entity(entity_manager, "Entity 4");
	Entity e5 = create_entity(entity_manager, "Entity 5");

	attach_child_entity(entity_manager, e1, e2);
	attach_child_entity(entity_manager, e2, e3);
	attach_child_entity(entity_manager, e1, e4);

	

	

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

	//SDL_ShowCursor(SDL_ENABLE);
	//SDL_SetRelativeMouseMode(SDL_FALSE);

	if (window_has_focus(window) && editor->was_last_frame_using_right_click) {
		if (!editor->did_last_frame_have_window_focus) {
			cmd_editor_window_focus_change(editor, true);
		}		
		editor->did_last_frame_have_window_focus = true;
	} else {
		if (editor->did_last_frame_have_window_focus) {
			cmd_editor_window_focus_change(editor, false);
		}
		editor->did_last_frame_have_window_focus = false;
	}
	
	// Skip input handling if the window is not focused
	if (!window_has_focus(window)) {
		//SDL_ShowCursor(SDL_ENABLE);
		//SDL_SetRelativeMouseMode(SDL_FALSE);
		editor->was_last_frame_using_right_click = false;
		return;
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
		Vec3f cam_pos = position(entity_manager, editor->editor_camera);
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


	

	// Only apply editor movement if right mouse button is clicked
	if (is_mouse_down(input, MouseButton::Right)) {
		//SDL_SetWindowGrab(window->sdl_window, SDL_TRUE);
		SDL_ShowCursor(SDL_DISABLE);
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


s32 editor_recv_callback(void* params) {

	struct Params {
		char* buf;
		size_t buf_len;
		void* params;
	};

	Params* p;
	p = (Params*)params;
	
	EditorInterface* editor = (EditorInterface*)p->params;


	char* buf = p->buf;
	size_t buf_len = p->buf_len;

	EditorCommandQueue* ecq = &editor->ecq;



	
	// Just keep reading bytes into our buffer then once we have enough bytes to know how big the message size is
	// we'll be able to determine how big our message is
	memcpy(ecq->packet_buffer + ecq->packet_bytes_read, buf, buf_len);
	ecq->packet_bytes_read += buf_len;

	if (ecq->packet_bytes_read > sizeof(EditorCommandHeader)) {
		// Since we know we've read more than the header, we can safely get the expected message size

		//char* hbuf = editor->ecq.packet_buffer;
		//EditorCommandHeader header;
		//memcpy(header.magic, hbuf, 4);
		//hbuf += 4;
		//hbuf = read_s32(editor->ecq.packet_buffer, &header.version);
		//hbuf = read_u64(editor->ecq.packet_buffer, &header.message_size);
		
		ecq->command_expected_size = (size_t)p_ntohll((*(u64*)(ecq->packet_buffer + 8)));
	}

	if (ecq->packet_bytes_read >= ecq->command_expected_size + sizeof(EditorCommandHeader)) {
		// We've read more data than we need for a command
		// parse the entire buffer, and generate a command, and put it into the command queue
		EditorCommandHeader header;

		char* hbuf = ecq->packet_buffer;
		// Read magic bytes
		memcpy(header.magic, hbuf, MAGIC_BYTES_SIZE);
		hbuf += MAGIC_BYTES_SIZE;

		// Read version, and re read in the header message size
		hbuf = read_s32(hbuf, &header.version);
		hbuf = read_u64(hbuf, &header.message_size);

		
		// Parse command from buffer
		EditorCommand command;

		s32 cmd_type;
		s32 undo_cmd_type;

		hbuf = read_s32(hbuf, &cmd_type);
		hbuf = read_s32(hbuf, &undo_cmd_type);
		command.type = (EditorCommandType)cmd_type;
		command.undo_type = (EditorCommandType)undo_cmd_type;
		hbuf = read_u64(hbuf, &command.buffer_size);

		command.buffer = (char*)stack_alloc(&editor->stack, command.buffer_size, 1);
		memcpy(command.buffer, hbuf, command.buffer_size);
		process_command(editor, command);
		stack_pop(&editor->stack);
		



		// Reset packet reading
		editor->ecq.packet_bytes_read = 0;
		editor->ecq.command_expected_size = 0;
	}


	
	


	debug_print("recv callback");
		
		
	return 0;
}

void connect_editor_socket(EditorInterface* editor) {
	//editor->listen_socket = p_sock_create_handle();
	//p_sock_bind(editor->listen_socket);
	//p_sock_listen(editor->listen_socket);
	//p_sock_accept(editor->listen_socket);
	editor->listen_socket = p_setup_sockets_async(&editor_recv_callback, (void*)editor);
	
	attach_logger_to_editor(editor);
	

	Window* window = editor->api.window;
}

void disconnect_editor_socket(EditorInterface* editor) {
	editor->connection_status = EditorConnectionStatus::Disconected;
	p_sock_close(editor->listen_socket);
	p_sock_shutdown(editor->listen_socket);
	p_sock_free_handle(editor->listen_socket);

}


static void attach_logger_to_editor(EditorInterface* editor) {
	// pass the editor as a data param for the callback
	g_attach_logger_callback(editor_logger_callback, (void*)editor);
}

static void editor_logger_callback(void* data) {

	struct {
		EditorInterface* editor;
		LogItem* item;
	} param;

	// The data is the mergd param data
	// Since we know what the expected data is, we can hardcode the un packing of the data
	// First part of the data is the editor we passed in.

	// Ugly and hardcoded, but we know that when the callback is being called, what the exact layout of the param struct is
	memcpy(&param, data, sizeof(param));
	
	cmd_send_log_to_editor(param.editor, param.item);
	
}







static void send_command_to_editor(EditorInterface* editor, EditorCommand command) {

	
		

	EditorCommandHeader header;
	header.version = EDITOR_COMMAND_VERSION;
	// NOTE: we explicity define the message size here. Using sizeof(command) works
	// but in cases the compiler will put padding in cases when we add new data to the command struct
	// better to explicity define the header size.
	header.message_size = 
		sizeof(command.type) // type
		+ sizeof(command.undo_type) // undo type
		+ sizeof(command.buffer_size)
		+ command.buffer_size;
	
	
	// NOTE: we explicity define the header size here. Using sizeof(header) works
	// but in cases the compiler will put padding in cases when we add new data to the header struct
	// better to explicity define the header size.
	size_t header_size =
		+ sizeof(header.magic)
		+ sizeof(header.version)
		+ sizeof(header.message_size);

	size_t msg_size = header.message_size;

	size_t buf_size = header_size + msg_size;





	char* buf_start = (char*)stack_alloc(&editor->stack, buf_size, 1);
	char* buf = buf_start;

	
	// Write header
	char magic[MAGIC_BYTES_SIZE] = MAGIC_BYTES;
	memcpy(buf, magic, MAGIC_BYTES_SIZE);
	buf += MAGIC_BYTES_SIZE;

	buf = write_s32(buf, header.version);
	buf = write_u64(buf, (u64)header.message_size);

	// Write message
	// NOTE: since we are using enum class/struct with a specified type of s32, this is safe
	// without this, enums integral types are implementation defined.
	buf = write_s32(buf, (s32)command.type);
	buf = write_s32(buf, (s32)command.undo_type);
	buf = write_u64(buf, (u64)command.buffer_size);
	memcpy(buf, command.buffer, command.buffer_size);
	
	
	// NOTE: if u make this async, then u can't pop the stack right away
	p_sock_send(editor->listen_socket, buf_start, buf_size, 0);
	stack_pop(&editor->stack);

}

static void process_command(EditorInterface* editor, EditorCommand command) {
	switch (command.type) {
		case EditorCommandType::ENGINE_CONNECT : {
			SDL_SysWMinfo wmInfo;
			SDL_VERSION(&wmInfo.version);
			SDL_GetWindowWMInfo(editor->api.window->sdl_window, &wmInfo);
			HWND hwnd = wmInfo.info.win.window;
			debug_print("%d\n", (u64)hwnd);

			cmd_request_engine_connect(editor, (u64)hwnd);
			break;
		}
		case EditorCommandType::ENGINE_DISCONNECT: {
			editor->connection_status = EditorConnectionStatus::Disconected;
			break;
		}
		case EditorCommandType::ENGINE_DATA: {
			cmd_respond_engine_data(editor);
			break;
		}
		default:
			break;
	}

}


static char* write_u32(char* buffer, u32 value) {
	*(u32*)buffer = p_htonl(value);
	return buffer + sizeof(u32);
}

static char* write_s32(char* buffer, s32 value) {
	*(s32*)buffer = p_htonl(value);
	return buffer + sizeof(s32);
}

static char* read_u32(char* buffer, u32* value) {
	*value = p_ntohl(*(u32*)buffer);
	return buffer + sizeof(u32);
}

static char* read_s32(char* buffer, s32* value) {
	*value = p_ntohl(*(s32*)buffer);
	return buffer + sizeof(s32);
}

static char* write_u64(char* buffer, u64 value) {
	*(u64*)buffer = p_htonll(value);
	return buffer + sizeof(u64);
}

static char* write_s64(char* buffer, s64 value) {
	*(s64*)buffer = p_htonll(value);
	return buffer + sizeof(s64);
}

static char* read_u64(char* buffer, u64* value) {
	*value = p_ntohll(*(u64*)buffer);
	return buffer + sizeof(u64);
}

static char* read_s64(char* buffer, s64* value) {
	*value = p_ntohll(*(s64*)buffer);
	return buffer + sizeof(s64);
}

static char* write_buf(char* dst_buffer, const char* src_buffer, size_t src_buf_size) {
	memcpy(dst_buffer, src_buffer, src_buf_size);
	return dst_buffer + src_buf_size;
}


static void cmd_request_engine_connect(EditorInterface* editor, u64 hwnd) {
	EditorCommand command;
	command.type = EditorCommandType::ENGINE_CONNECT;
	command.undo_type = EditorCommandType::NONE;
	command.buffer_size = sizeof(hwnd);
	command.buffer = (char*)stack_alloc(&editor->stack, command.buffer_size, 1);

	char* buffer = command.buffer;
	buffer = write_u64(command.buffer, hwnd);
	

	// NOTE: if u make sending commands async, than u can't pop stack right after calling it
	send_command_to_editor(editor, command);
	stack_pop(&editor->stack);
	editor->connection_status = EditorConnectionStatus::Connected;

	
}


static void cmd_respond_engine_data(EditorInterface* editor) {
	EditorCommand command;
	command.type = EditorCommandType::ENGINE_DATA;
	command.undo_type = EditorCommandType::NONE;
	command.buffer_size = 0;
	command.buffer = (char*)stack_alloc(&editor->stack, KILOBYTES(1), 1);


	char* buffer = command.buffer;
	EntityManager* entity_manager = editor->api.entity_manager;
	u64 e_count = entity_count(entity_manager);
	

	// The buffer will be structured as
	// first 8 bytes will be u64. This holds how many entitys we have inside the entity_manager
	// after this, will be a list of the entity id, its name, its parent entity, and its children 
	// an example of a entity list item would be
	
	// Entity id|Entity name length|Entity name|Parent Entity|Children count|list of children entity ids|
	//         1|                 8|"Entity 1" |            0|             3|                   21,33,44|
	// NOTE: that in the binary buffer there are no "|" as seperators.

	// 8 bytes for Entity id
	// 8 bytes for Entity name length. NOTE: string stores a size_t for the name length. However in the end we will still write it as a u64
	// Entity name length bytes for the Entity name
	// 8 bytes for the parent entity id
	// 8 bytes for the children count
	// 8 * Children count bytes to hold the children entity ids
	
	
	// example buffer
	// 4 entitys
	// 1|8|"Entity 1"|0|3|21,33,44|
	// 21|9|"Entity 21"|0|0||
	// 33|9|"Entity 33"|0|0||
	// 44|9|"Entity 44"|0|0||

	// NOTE: 0 for the parent entity means it is the root

	




	// Write entity count
	buffer = write_u64(buffer, e_count);

	command.buffer_size += sizeof(e_count);

	for (u64 i = 0; i < e_count; i++) {
		Entity entity = entity_manager->entity_list[i];
		String name = get_name(&entity_manager->transform_manager, entity);
		u64 child_count = children_count(entity_manager, entity);
		Entity parent_entity = parent(entity_manager, entity);

		// Write entity id
		buffer = write_u64(buffer, entity.id);
		// Write name string length
		buffer = write_u64(buffer, (u64)name.length);
		// Write name string
		buffer = write_buf(buffer, name.buffer, name.length);
		// Write parent entity
		buffer = write_u64(buffer, parent_entity.id);
		// Write child count
		buffer = write_u64(buffer, child_count);

		command.buffer_size += sizeof(entity.id);
		command.buffer_size += sizeof(name.length);
		command.buffer_size += name.length;
		command.buffer_size += sizeof(parent_entity.id);
		command.buffer_size += sizeof(child_count);

		// Write child entity ids
		Entity child = first_child(entity_manager, entity);
		while (child.id != NO_ENTITY_ID) {
			buffer = write_u64(buffer, child.id);
			command.buffer_size += sizeof(child.id);
			child = next_sibling(entity_manager, child);
		}
	}


	

	//char* buffer = command.buffer;
	//buffer = write_u64(command.buffer, hwnd);

	//get all engine data and push it down the socket
	//
	//	- scene data
	//	- assets
	//
	//editor->api.entity_manager->
	
	send_command_to_editor(editor, command);
	stack_pop(&editor->stack);
	
}

static void cmd_send_log_to_editor(EditorInterface* editor, LogItem* item) {
	EditorCommand command;
	command.type = EditorCommandType::SEND_LOG_ITEM;
	command.undo_type = EditorCommandType::NONE;
	command.buffer_size =
		sizeof(item->verbosity)
		+ sizeof(item->time)
		+ sizeof(item->thread_id)
		+ sizeof(item->line)
		// Tag string
		+ sizeof(item->tag_length)
		+ item->tag_length
		// Function string
		+ sizeof(item->function_length)
		+ item->function_length
		// Filename string
		+ sizeof(item->filename_length)
		+ item->filename_length
		// Message string
		+ sizeof(item->msg_length)
		+ item->msg_length;

	command.buffer = (char*)stack_alloc(&editor->stack, command.buffer_size, 1);

	char* buffer = command.buffer;
	buffer = write_u32(buffer, (u32)item->verbosity);
	buffer = write_u64(buffer, item->time);
	
	buffer = write_s32(buffer, item->thread_id);
	buffer = write_s32(buffer, item->line);

	// write tag name
	buffer = write_s32(buffer, item->tag_length);
	buffer = write_buf(buffer, item->tag, item->tag_length);

	// write function name
	buffer = write_u64(buffer, item->function_length);
	buffer = write_buf(buffer, item->function, item->function_length);

	// write filename name
	buffer = write_u64(buffer, item->filename_length);
	buffer = write_buf(buffer, item->filename, item->filename_length);
	// write msg
	buffer = write_u64(buffer, item->msg_length);
	buffer = write_buf(buffer, item->msg, item->msg_length);
	
	send_command_to_editor(editor, command);
	stack_pop(&editor->stack);
}

static void cmd_editor_window_focus_change(EditorInterface* editor, bool has_focus) {
	EditorCommand command;
	command.type = EditorCommandType::EDITOR_WINDOW_FOCUS_CHANGE;
	command.undo_type = EditorCommandType::NONE;
	command.buffer_size = sizeof(s32);
		

	command.buffer = (char*)stack_alloc(&editor->stack, command.buffer_size, 1);

	char* buffer = command.buffer;
	s32 has_focus_int = has_focus ? 1 : 0;
	buffer = write_s32(buffer, has_focus);
	

	send_command_to_editor(editor, command);
	stack_pop(&editor->stack);

}