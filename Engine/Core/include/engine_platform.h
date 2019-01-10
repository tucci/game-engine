#pragma once



#include "types.h"


struct FileHandle;
struct SocketHandle;



#ifdef _WIN32
#else
//typedef struct FileHandle {
//	int linux_handle;
//} FileHandle;
//
//typedef struct SocketHandle {
//	int socket;
//} SocketHandle;

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



s64 platform_get_pid();
s64 platform_get_thread_id();


// Platform implemented vscprintf
int p_vscprintf(const char * format, va_list pargs);
// Platform implemented scprintf
int p_scprintf(const char *format, ...);
// Platform implemented vsprintf_s
int p_vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list vlist);


// Platform sockets



int sock_init();
SocketHandle sock_create();
void sock_bind(SocketHandle handle);
void sock_listen(SocketHandle handle);
void sock_accept(SocketHandle handle);
int sock_close(SocketHandle handle);
void sock_shutdown(SocketHandle handle);
int sock_send(SocketHandle handle, char* buf, int len, int flags);
int sock_recv(SocketHandle handle, char* buf, int len, int flags);

