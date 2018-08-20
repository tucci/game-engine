#pragma once

#include <stdio.h>
#include <stdbool.h>


#include "Common/LinearAllocator.h"

inline char* file_to_str(const char* filename, LinearAllocator* mem) {
	char * buffer = 0;
	long length;


	FILE* file;
	errno_t err;

	err = fopen_s(&file, filename, "r");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		debug_print("The file was opened\n");
	} else {
		return NULL;
		debug_print("The file was not opened\n");
	}

	
	
	
	if (file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = linear_alloc(mem, length, 1);
		if (buffer) {
			fread(buffer, 1, length, file);
		}
		fclose(file);
	}

	return buffer;
}