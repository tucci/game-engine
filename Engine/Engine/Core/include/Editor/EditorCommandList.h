#pragma once

#include "types.h"
#include "Core/ECS/Entity.h"

struct EditorInterface;

#define EDITOR_COMMAND_VERSION 1

enum struct EditorCommandType : s32 {
	NONE = 0,
	// Used for group command
	COMMAND_GROUP_START = 1,
	COMMAND_GROUP_END = 2,
	COMMAND_FORCE_NO_MERGE = 3,

	NEW_ENTITY,
	DELETE_ENTITY,
	SELECT_ENTITY,
	DUPLICATE_ENTITY,
	SET_TRANSFORM_COMPONENT,
	SET_STATICMESH_COMPONENT,
	SET_MATERIAL_COMPONENT,
	SET_LIGHT_COMPONENT,
	SET_CAMERA_COMPONENT,
	REPARENT_ENTITY,
};








static void cmd_editor_group_begin(EditorInterface* editor);
static void cmd_editor_group_end(EditorInterface* editor);
static void cmd_editor_force_no_merge(EditorInterface* editor);

static void cmd_editor_create_emtpy_entity(EditorInterface* editor);
static void cmd_editor_create_plane(EditorInterface* editor);
static void cmd_editor_create_sphere(EditorInterface* editor);
static void cmd_editor_create_cube(EditorInterface* editor);
static void cmd_editor_create_light(EditorInterface* editor);
static void cmd_editor_create_camera(EditorInterface* editor);


static void cmd_editor_select_entity(EditorInterface* editor, Entity entity, bool selected);
static void cmd_editor_deselect_all_entitys(EditorInterface* editor);

static void cmd_edtior_set_transform_component(EditorInterface* editor, Entity e, Vec3f old_pos, Quat old_rot, Vec3f old_scale, Vec3f pos, Quat rot, Vec3f scale, bool merge_if_top);
static void cmd_editor_set_staticmesh_component(EditorInterface* editor, Entity e, StaticMeshID old_id, StaticMeshID new_id);
static void cmd_editor_set_material_component(EditorInterface* editor, Entity e, MaterialID old_id, MaterialID new_id);
static void cmd_editor_set_light_component(EditorInterface* editor, Entity e, Light old_light, Light new_light, bool merge_if_top);
static void cmd_editor_set_camera_component(EditorInterface* editor, Entity e, Camera old_camera, Camera new_camera, bool merge_if_top);

static void cmd_editor_reparent_entity(EditorInterface* editor, Entity e, Entity old_parent, Entity new_parent);
