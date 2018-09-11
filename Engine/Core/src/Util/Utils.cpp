#pragma once

#include "Util/Utils.h"

#include "debug_macros.h"
#include "Common/common_macros.h"

char* file_to_str(const char* filename, StackAllocator* mem) {
	char * buffer = 0;
	long length;


	FILE* file;
	errno_t err;

	err = fopen_s(&file, filename, "rb");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		debug_print("The file was opened\n");
	} else {
		debug_print("The file was not opened\n");
		return NULL;
	}


	//problem is that ftell is couting number of \n

	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		// + 1 for the \0 terminator
		buffer = cast(char*) stack_alloc(mem, length + 1, 1);
		if (buffer) {
			fread(buffer, 1, length, file);
		}
		// Terminate the string
		buffer[length] = '\0';
		fclose(file);
	}

	return buffer;
}