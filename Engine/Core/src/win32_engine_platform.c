#pragma once


#ifdef _WIN32

#include "engine_platform.h"


#include "Shlwapi.h"



void convert_to_os_path(const char* path, char* output_buffer, s32 output_buffer_length) {
	const char* stream = path;
	int buffer_index = 0;
	// Prevent buffer overrun
	while (*stream != NULL && buffer_index < output_buffer_length - 1) {
		if (*stream == '/') {

			output_buffer[buffer_index] = '\\';
			//output_buffer[buffer_index + 1] = '\\';
			buffer_index += 1;
		} else {
			output_buffer[buffer_index] = *stream;
			buffer_index++;
		}
		stream++;
	}
	output_buffer[buffer_index] = '\0';
}


FileHandle platform_create_file(const char* filename) {
	assert(filename != NULL);
	FileHandle fh;
	HANDLE h = CreateFile(
		filename,
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0
	);

	if (h != INVALID_HANDLE_VALUE) {
		fh.win_handle = h;
	} else {
		// error
		fh.win_handle = NULL;
	}


	return fh;
}

bool platform_close_file(FileHandle handle) {
	return CloseHandle(handle.win_handle);
}

u32 platform_path(FileHandle handle, char* path, u32 path_length) {
	assert(path != NULL);

	DWORD length_of_actual_path = GetFinalPathNameByHandle(
		handle.win_handle,
		path,
		path_length,
		VOLUME_NAME_NONE
	);
	return length_of_actual_path;
}

s64 platform_get_file_size(FileHandle handle) {
	LARGE_INTEGER size;
	if (GetFileSizeEx(handle.win_handle, &size)) {
		return (s64)size.QuadPart;
	}
	return -1;	
}

const char* platform_file_basename(const char* filename) {
	assert(filename != NULL);
	LPCSTR file_str = filename;
	LPCSTR result = PathFindFileName(file_str);
	return result;
}


bool platform_file_dirname(IString path, char* output_path, s32 output_path_length) {
	assert(output_path != NULL);
	// First check to see if the actual path is bigger than the output buffer, to prevent buffer overrun
	if (path.length > output_path_length) {
		return false;
	}
	convert_to_os_path(path.buf, output_path, output_path_length);
	//memcpy(output_path, path.buf, path.length);
	//output_path[path.length] = '\0';
	return PathRemoveFileSpec(output_path);
}

bool platform_concat_path_and_filename(IString path, IString filename, char* output, s32 output_length) {
	assert(output != NULL);
	s32 concat_length = path.length + filename.length + 1;
	if (concat_length > output_length) {
		return false;
	}
	char* result = PathCombine(output, path.buf, filename.buf);
	// Compare pointer values,
	assert(result == output);
	if (result == NULL) {
		return false;
	}
	return true;

}


bool platform_file_rename(const char* existing_file, const char* new_file) {
	assert(existing_file != NULL);
	assert(new_file != NULL);
	return MoveFile(existing_file, new_file);
}

bool platform_file_delete(const char* filename) {
	assert(filename != NULL);
	return DeleteFile(filename);
}

bool platform_pathfile_exists(const char* path) {
	assert(path != NULL);
	return PathFileExists(path);
}

s64 platform_get_pid() {
	DWORD pid = GetCurrentProcessId();
	return pid;
}

s64 platform_get_thread_id() {
	DWORD pid = GetCurrentThreadId();
	return pid;
}

int p_vscprintf(const char * format, va_list pargs) {
	int count = _vscprintf(format, pargs);
	return count;
}

int p_scprintf(const char *format, ...) {
	
	va_list args;
	va_start(args, format);
	int count = p_vscprintf(format, args);
	va_end(args);
	return count;
}

int p_vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list vlist) {
	int result = vsprintf_s(buffer, numberOfElements, format, vlist);
	return result;
}


#endif