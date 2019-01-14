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

typedef struct EditorInterface {
	bool was_last_frame_using_right_click;
	EngineAPI api;
	Arena arena;
	StackAllocator stack;



	Entity editor_camera;
	HDR_SkyMap hdr_skymap;

	SocketHandle* socket;
	
} EditorInterface;





bool init_editor_interface(EditorInterface* editor, EngineAPI api);
void destroy_editor_interface(EditorInterface* editor);


void connect_editor_socket(EditorInterface* editor);
void disconnect_editor_socket(EditorInterface* editor);
void editor_update(EditorInterface* editor);

void send_command_to_editor(EditorInterface* editor, EditorCommand command);

