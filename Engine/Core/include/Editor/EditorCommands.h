#pragma once

#include "types.h"




#define EDITOR_COMMAND_VERSION 1

#define MAGIC_BYTES_SIZE 4

// 1 2 3 4 for big endian and
// 4 3 2 1 for little endian
#define MAGIC_BYTES {'1', '2', '3', '4'}

enum struct EditorCommandType : s32 {
	NONE,
	REQUEST_ENGINE_CONNECT,
	REQUEST_ENGINE_DISCONNECT
};

struct EditorCommandHeader {
	// Header
	// The magic bytes tells the parser what type of what this packet is for
	// also can be used to check endianness
	char magic[MAGIC_BYTES_SIZE];
	s32 version;
	//s32 compression;
	size_t message_size;
};

struct EditorCommand {

	// Message
	EditorCommandType type;
	EditorCommandType undo_type;
	size_t buffer_size;
	char* buffer;
};




