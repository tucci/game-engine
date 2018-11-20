#pragma once


#ifdef _WIN32

#include "engine_platform.h"


#include "Shlwapi.h"


const char* platform_file_basename(const char* filename, int filename_length) {
	LPCSTR file_str = filename;
	LPCSTR result = PathFindFileNameA(file_str);
	return result;
}

#endif