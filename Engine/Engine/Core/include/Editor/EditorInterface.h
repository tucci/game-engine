#pragma once


#include "engine_platform.h"

#include "EditorCommands.h"

#include "Core/EngineAPI.h"
#include "Logger.h"


#include "Core/ECS/Component/Primitives.h"

#include "Asset/AssetManager.h"
#include "Asset/FbxImport.h"


#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"



#define EDITOR_MEMORY MEGABYTES(100)




#define FPS_HISTORY_COUNT 240
struct EditorInterface {

	EngineAPI api;
	Arena arena;
	StackAllocator stack;

	CompactMap<bool> entity_selected;


	

	Entity editor_camera;
	Entity entity_test_light;
	Entity test_mesh;
	
	
	HDR_SkyMap hdr_skymap;

	bool show_editor;


	bool was_last_frame_using_right_click;

	// Game clock vars
	int fps_history_index = 0;
	float fps_history[FPS_HISTORY_COUNT] = { 0 };

	// Log vars
	bool show_info;
	bool show_warning;
	bool show_fatal;

	int log_start_offset;

	bool show_time;
	bool show_tag;
	bool show_thread_id;
	bool show_filename;
	bool show_function;
	bool show_line;
	bool show_message;

	
	bool auto_scroll;

	bool window_scene_tree_open;
	bool window_log_open;
	bool window_asset_browser_open;
	bool window_entity_components_open;
	bool window_engine_timers_open;
	bool window_render_stats;
	


	
};





bool init_editor_interface(EditorInterface* editor, EngineAPI api);
void destroy_editor_interface(EditorInterface* editor);

void editor_update(EditorInterface* editor);

static void draw_component_transform(EditorInterface* editor, Entity e);
static void draw_component_camera(EditorInterface* editor, Entity e);
static void draw_component_light(EditorInterface* editor, Entity e);
static void draw_component_static_mesh(EditorInterface* editor, Entity e);
static void draw_component_render(EditorInterface* editor, Entity e);



static void draw_main_menu_bar(EditorInterface* editor);
static void draw_window_entity_components(EditorInterface* editor);
static void draw_window_scene_hierarchy(EditorInterface* editor);
static void draw_window_engine_timer(EditorInterface* editor);
static void draw_window_log(EditorInterface* editor);
static void draw_window_assets(EditorInterface* editor);

static void draw_window_renderer_stats(EditorInterface* editor);

static void draw_window_scene_viewports(EditorInterface* editor);






