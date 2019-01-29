#pragma once


#include "engine_platform.h"
#include "Editor/EditorCommands.h"

#include "Core/EngineAPI.h"


#include "Core/ECS/Component/Primitives.h"


#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"


#define ENGINE_MODE_EDITOR 1

#define EDITOR_MEMORY MEGABYTES(100)


#define EDITOR_COMMAND_RECV_BUFFER_SIZE 64

struct EditorCommandQueue {
	size_t command_expected_size;
	size_t packet_bytes_read;
	char* packet_buffer;
};

struct EditorInterface {
	bool was_last_frame_using_right_click;
	EngineAPI api;
	Arena arena;
	StackAllocator stack;



	Entity editor_camera;
	HDR_SkyMap hdr_skymap;
	SocketHandle* listen_socket;

	EditorCommandQueue ecq;
	
};





bool init_editor_interface(EditorInterface* editor, EngineAPI api);
void destroy_editor_interface(EditorInterface* editor);


void connect_editor_socket(EditorInterface* editor);
void disconnect_editor_socket(EditorInterface* editor);
void editor_update(EditorInterface* editor);

static void send_command_to_editor(EditorInterface* editor, EditorCommand command);

static void process_command(EditorInterface* editor, EditorCommand command);


static char* write_u32(char* buffer, u32 value);
static char* write_s32(char* buffer, s32 value);
static char* read_u32(char* buffer, u32* value);
static char* read_s32(char* buffer, s32* value);




static char* write_u64(char* buffer, u64 value);
static char* write_s64(char* buffer, s64 value);
static char* read_u64(char* buffer, u64* value);
static char* read_s64(char* buffer, s64* value);


void cmd_request_engine_connect(EditorInterface* editor, u64 hwnd);

