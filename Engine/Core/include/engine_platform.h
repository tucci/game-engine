#pragma once



#include "types.h"


#ifdef _WIN32
#include "Shlwapi.h"

typedef struct FileHandle {
	HANDLE win_handle;
} FileHandle;


#else
typedef struct FileHandle {
	int linux_handle;
} FileHandle;
#endif

void convert_to_os_path(const char* path, char* output_buffer, s32 output_buffer_length);

FileHandle platform_create_file(const char* filename);

bool platform_close_file(FileHandle handle);

u32 platform_path(FileHandle handle, char* path, u32 path_length);


s64 platform_get_file_size(FileHandle handle);


const char* platform_file_basename(const char* filename);
bool platform_file_dirname(IString path, char* output_path, s32 output_path_length);
bool platform_concat_path_and_filename(IString path, IString filename, char* output, s32 output_length);

bool platform_file_rename(const char* existing_file, const char* new_file);
bool platform_file_delete(const char* filename);

bool platform_pathfile_exists(const char* path);



