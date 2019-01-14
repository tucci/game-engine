#pragma once

#include "Editor/EditorCommands.h"

EditorCommand cmd_send_window(u32 hwnd) {
	EditorCommand command;
	command.type = EditorCommand_ESTABLISH_CONNECTION;
	command.hwnd = (u32)hwnd;
	return command;
}

static void write_int32(EditorCommand command, s32 value) {
	
}