#pragma once


#include "engine_platform.h"

#include "EditorCommands.h"

#include "Core/EngineAPI.h"
#include "Logger.h"


#include "Core/ECS/Component/Primitives.h"


#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"


#define ENGINE_MODE_EDITOR 1

#define EDITOR_MEMORY MEGABYTES(100)



struct EditorCommandQueue {
	size_t command_expected_size;
	size_t packet_bytes_read;
	char* packet_buffer;
};

enum class EditorConnectionStatus {
	None,
	Connected,
	Disconected
};

struct EditorInterface {
	bool was_last_frame_using_right_click;
	bool did_last_frame_have_window_focus;
	EngineAPI api;
	Arena arena;
	StackAllocator stack;



	Entity editor_camera;
	HDR_SkyMap hdr_skymap;
	SocketHandle* listen_socket;

	EditorCommandQueue ecq;

	EditorConnectionStatus connection_status;
	
};





bool init_editor_interface(EditorInterface* editor, EngineAPI api);
void destroy_editor_interface(EditorInterface* editor);


void connect_editor_socket(EditorInterface* editor);
void disconnect_editor_socket(EditorInterface* editor);

static void attach_logger_to_editor(EditorInterface* editor);
static void editor_logger_callback(void* data);

void editor_select_entity(EditorInterface* editor, Entity entity);
void editor_update(EditorInterface* editor);

static void send_command_to_editor(EditorInterface* editor, EditorCommand command);

static void process_command(EditorInterface* editor, EditorCommand command);


// TODO: Move these out to a seperate de/serialzer file
static char* write_u32(char* buffer, u32 value);
static char* write_s32(char* buffer, s32 value);
static char* read_u32(char* buffer, u32* value);
static char* read_s32(char* buffer, s32* value);

static char* write_u64(char* buffer, u64 value);
static char* write_s64(char* buffer, s64 value);
static char* read_u64(char* buffer, u64* value);
static char* read_s64(char* buffer, s64* value);

static char* write_buf(char* dst_buffer, const char* src_buffer, size_t src_buf_size);



// Outgoing commands that the engine sends to the editor
static void cmd_from_engine_send_logitem(EditorInterface* editor, LogItem* item);
static void cmd_from_engine_window_focus_change(EditorInterface* editor, bool has_focus);

// Incoming commands that the editor sent to the engine
// The engine may send back another command indicating that it got the command, with any other data
static void cmd_from_editor_request_engine_connect(EditorInterface* editor, EditorCommand* recv_command);
static void cmd_from_editor_get_scene_hierarchy(EditorInterface* editor, EditorCommand* recv_command);
static void cmd_from_editor_new_entity(EditorInterface* editor, EditorCommand* recv_command);
static void cmd_from_editor_delete_entities(EditorInterface* editor, EditorCommand* recv_command);
static void cmd_from_editor_select_entity_list(EditorInterface* editor, EditorCommand* recv_command);


static void cmd_from_editor_duplicate_entity_list(EditorInterface* editor, EditorCommand* recv_command);


