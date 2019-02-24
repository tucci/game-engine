#pragma once


#include "engine_platform.h"
#include "Editor/EditorCommands.h"

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


static void cmd_request_engine_connect(EditorInterface* editor, u64 hwnd);
static void cmd_respond_engine_data(EditorInterface* editor);
static void cmd_send_log_to_editor(EditorInterface* editor, LogItem* item);

static void cmd_editor_window_focus_change(EditorInterface* editor, bool has_focus);


