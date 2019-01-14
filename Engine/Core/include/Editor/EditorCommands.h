#pragma once

#include "types.h"

typedef enum EditorCommandType {
	EditorCommand_NONE,
	EditorCommand_ESTABLISH_CONNECTION
} EditorCommandType;


#define EDITOR_COMMAND_VESRSION 1

typedef struct EditorCommandHeader {
	// NOTE: if you change the size of the header, make sure you have everything padded
	// Header
	s32 endianness;
	s32 version;
	//s32 compression;
	s32 message_size;
} EditorCommandHeader;


typedef struct EditorCommand {

	// Message
	EditorCommandType type;
	//s32 buffer_size;
	//char* buffer;
	union {
		u32 hwnd;
	};
} EditorCommand;




EditorCommand cmd_send_window(u32 hwnd);

static void write_int32(EditorCommand command, s32 value);