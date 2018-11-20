#pragma once


#ifdef _WIN32

#include "engine_platform.h"


#include "Shlwapi.h"


const char* platform_file_basename(const char* filename) {
	LPCSTR file_str = filename;
	LPCSTR result = PathFindFileNameA(file_str);
	return result;
}


bool platform_file_rename(const char* existing_file, const char* new_file) {
	return MoveFileA(existing_file, new_file);
}

bool platform_file_delete(const char* filename) {
	return DeleteFileA(filename);
}

bool platform_pathfile_exists(const char* path) {
	return PathFileExistsA(path);
}

#endif