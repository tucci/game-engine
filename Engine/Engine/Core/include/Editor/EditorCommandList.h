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

	NEW_ENTITY,
	DELETE_ENTITY,
	SELECT_ENTITY,
	DUPLICATE_ENTITY,
	SET_TRANSFORM,
};








static void cmd_editor_group_begin(EditorInterface* editor);
static void cmd_editor_group_end(EditorInterface* editor);

static void cmd_editor_create_emtpy_entity(EditorInterface* editor);
static void cmd_editor_create_plane(EditorInterface* editor);
static void cmd_editor_create_sphere(EditorInterface* editor);
static void cmd_editor_create_cube(EditorInterface* editor);
static void cmd_editor_create_light(EditorInterface* editor);
static void cmd_editor_create_camera(EditorInterface* editor);

static void cmd_editor_select_entity(EditorInterface* editor, Entity entity, bool selected);
static void cmd_editor_deselect_all_entitys(EditorInterface* editor);
static void cmd_edtior_set_transform(EditorInterface* editor, Entity e, Vec3f old_pos, Quat old_rot, Vec3f old_scale, Vec3f pos, Quat rot, Vec3f scale);
