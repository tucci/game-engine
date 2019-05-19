#pragma once

#ifdef linux

#include "engine_platform.h"

// TODO: LINUX platform


const char* platform_file_basename(const char* filename, int filename_length) {
	return filename;
}

#endif