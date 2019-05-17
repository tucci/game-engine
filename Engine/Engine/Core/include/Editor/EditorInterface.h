#pragma once


#include "engine_platform.h"


#include "Core/EngineAPI.h"
#include "Logger.h"

#include "EditorCommandList.h"

#include "Core/ECS/Component/Primitives.h"

#include "Asset/AssetManager.h"
#include "Asset/FbxImport.h"


#include "Core/ECS/JobSystem/TransformSystem.h"
#include "Core/ECS/JobSystem/CameraSystem.h"
#include "Core/ECS/JobSystem/StaticMeshSystem.h"
#include "Core/ECS/JobSystem/LightSystem.h"
#include "Core/ECS/JobSystem/RenderSystem.h"


#include "../../imgui/imgui.h"

#ifndef IMGUI_PROTO_API_USE
#define IMGUI_INTERNAL_API_USE
#include "../../imgui/imgui_internal.h"
#endif


#define EDITOR_MEMORY MEGABYTES(100)


struct EditorCommand_NewEntity {
	Entity entity;
	String name;
	ComponentType component_flags;
	StaticMeshID staticmesh_id;
	MaterialID material_id;
};

struct EditorCommand_DeleteEntity {
	/*instead of deleting a single entity, we rely on the fact that we may delete mulitple entitys at once in a tree form, we need to account for the tree linkage
	soo that when we need to undo this delete command, we know how to reconstruct it
	*/
	Entity entity_to_delete;
};

struct EditorCommand_SelectEntity {
	Entity entity_to_select;
	bool selected;
};


// We could store the transform matrix and perform the inverse
// That would use less size then storing old/new for each
// But this is much simpler, faster, easier to debug, and we can rollback to the old data without having to recompute all kinds of deltas
// and we dont have to worry about inverse quaterions and rotations
struct EditorCommand_SetTransformComponent {
	Entity entity;

	Vec3f position;
	Vec3f old_position;

	Vec3f scale;
	Vec3f old_scale;

	Quat rotation;
	Quat old_rotation;

};

struct EditorCommand_SetStaticMeshComponent {
	Entity entity;

	StaticMeshID old_id;
	StaticMeshID new_id;


};


struct EditorCommand_SetMaterialComponent {
	Entity entity;

	MaterialID old_id;
	MaterialID new_id;


};

struct EditorCommand_SetLightComponent {
	Entity entity;
	Light old_light;
	Light new_light;
};

struct EditorCommand_SetCameraComponent {
	Entity entity;
	Camera old_camera;
	Camera new_camera;
};

struct EditorCommand_ReparentEntity {
	Entity entity;
	Entity old_parent;
	Entity new_parent;
};


struct EditorCommandGroup {
	// stack_level indicates what the id/index/level the current group is in.
	// If the level is 0, then there is no parent groups
	// If the level > 0, then this group has a parent group that it is nested inside
	// Start 0
	//	Start 1
	//	End 1
	// End 0
	
	u64 stack_level = 0;
	bool in_group_transaction = false;
};

union EditorCommandData {
	EditorCommand_NewEntity new_entity;
	EditorCommand_DeleteEntity delete_entity;
	EditorCommand_SelectEntity select_entity;
	EditorCommand_SetTransformComponent set_transform;
	EditorCommand_SetStaticMeshComponent set_staticmesh;
	EditorCommand_SetMaterialComponent set_material;
	EditorCommand_SetLightComponent set_light;
	EditorCommand_SetCameraComponent set_camera;
	EditorCommand_ReparentEntity reparent_entity;
	EditorCommandGroup group;
	EditorCommandData() {};
};



struct EditorCommand {
	EditorCommandType type;
	EditorCommandData cmd;
};




#define EDITOR_COMMAND_BUFFER_CAPACITY 32

// TODO: this should probably not be fixed, but left as an option to the user to define how many undos they want to have
#define EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY 128
#define EDITOR_COMMAND_GROUP_STACK_COUNT 32



struct EditorCommandBuffer {

	//EditorCommandGroup current_group;
	

	size_t command_count = 0;
	size_t command_undo_stack_count = 0;
	size_t command_redo_stack_count = 0;
	size_t command_group_stack_count = 0;

	
	
	// How many commands can be send in one frame
	// After every frame, the commands are reset and set back to zero
	EditorCommand commands[EDITOR_COMMAND_BUFFER_CAPACITY];
	// This is the undo history of commands
	EditorCommand command_undo_stack[EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY];
	EditorCommand command_redo_stack[EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY];

	EditorCommandGroup group_stack[EDITOR_COMMAND_GROUP_STACK_COUNT];
};



struct EditorControlsData {
	enum {REPEAT_DELAY_TIME = 200, INCREMENT_REPEAT_TIME = 10, MAX_INCREMENT = (REPEAT_DELAY_TIME - INCREMENT_REPEAT_TIME)/ INCREMENT_REPEAT_TIME
	};
	u64 last_undo_time = 0;
	u64 undo_repeat_count = 0;
	u64 last_redo_time = 0;
	u64 redo_repeat_count = 0;
};

// Ideally We want to be able to have multiple browsers open
struct AssetBrowserData {
	// Asset Browser Data
	AssetBrowserFileNode* root;
	AssetBrowserFileNode* current_directory;
	


	ImGuiTextFilter asset_browser_filter;
	ImGuiTextFilter asset_tree_filter;
	bool asset_mesh_filter = true;
	bool asset_material_filter = true;
	bool asset_texture_filter = true;
	bool asset_scene_filter = true;
};




#define FPS_HISTORY_COUNT 240
struct EditorInterface {

	EngineAPI api;
	Arena arena;
	StackAllocator stack;

	u64 entity_selected_count = 0;
	CompactMap<bool> entity_selected;

	AssetImporter importer;
	EditorCommandBuffer cmd_buffer;
	
	AssetID test_mat;
	Entity editor_camera;
	Entity entity_test_light;
	Entity test_mesh;
	
	
	HDR_SkyMap hdr_skymap;

	EditorControlsData editor_control_data;

	RenderResource render_texture;
	Vec2i render_texture_size;
	RenderResource depth_texture;
	RenderResource render_framebuffer;

	RenderResource res_folder_icon_texture;
	RenderResource res_asset_icon_texture;

	Texture2D folder_icon_texture;
	Texture2D asset_icon_texture;

	

	ImGuiID dockspace_id = NULL;
	// Game clock vars
	int fps_history_index = 0;
	float fps_history[FPS_HISTORY_COUNT] = { 0 };

	AssetBrowserData asset_browser;
	
	bool right_click_down;
	bool scene_viewport_input_capture;
	

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

	ImGuiTextFilter scene_tree_entity_filter;
	ImGuiTextFilter log_filter;

	bool window_scene_tree_open;
	bool window_log_open;
	bool window_asset_browser_open;
	bool window_entity_components_open;
	bool window_engine_timers_open;
	bool window_render_stats;
	
};



static void push_editor_command(EditorInterface* editor, const EditorCommand& command);
static void perform_undo_operation(EditorInterface* editor);
static void perform_redo_operation(EditorInterface* editor);

static void perform_command(EditorInterface* editor, EditorCommand command, bool undo);
static void process_editor_command_buffer(EditorInterface* editor);





bool init_editor_interface(EditorInterface* editor, EngineAPI api);

void destroy_editor_interface(EditorInterface* editor);

void editor_update(EditorInterface* editor);
static bool is_entity_selected(EditorInterface* editor, Entity entity);

static void draw_component_transform(EditorInterface* editor, Entity e);
static void draw_component_camera(EditorInterface* editor, Entity e);
static void draw_component_light(EditorInterface* editor, Entity e);
static void draw_component_static_mesh(EditorInterface* editor, Entity e);
static void draw_component_render(EditorInterface* editor, Entity e);



static void draw_main_menu_bar(EditorInterface* editor);
static void draw_toolbar(EditorInterface* editor);
static void draw_window_entity_components(EditorInterface* editor);
static void draw_window_scene_hierarchy(EditorInterface* editor);
static void draw_window_engine_timer(EditorInterface* editor);
static void draw_window_log(EditorInterface* editor);
static void draw_window_assets(EditorInterface* editor);

static void draw_window_renderer_stats(EditorInterface* editor);
static void draw_viewports(EditorInterface* editor);
static void draw_editor_command_undo_and_redo_stack(EditorInterface* editor);







