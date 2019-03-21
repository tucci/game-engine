#pragma once



#include "types.h"
#include "debug_macros.h"


struct FileHandle;
struct SocketHandle;
struct ThreadHandle;

typedef void job_queue_callback(void* data);
struct JobQueue;


static const unsigned __int64 epoch = 116444736000000000LL;

void convert_to_os_path(const char* path, char* output_buffer, s32 output_buffer_length);

FileHandle* platform_create_filehandle(const char* filename);
void platform_free_filehandle(FileHandle* handle);
bool platform_close_file(FileHandle* handle);
u32 platform_path(FileHandle* handle, char* path, u32 path_length);
s64 platform_get_file_size(FileHandle* handle);


const char* platform_file_basename(const char* filename);
bool platform_file_dirname(IString path, char* output_path, s32 output_path_length);
bool platform_concat_path_and_filename(IString path, IString filename, char* output, s32 output_length);
const char* platform_file_extension(const char* filename);

bool platform_file_rename(const char* existing_file, const char* new_file);
bool platform_file_delete(const char* filename);

bool platform_pathfile_exists(const char* path);
s64 platform_ms_since_epoch();



s64 platform_get_pid();
s64 platform_get_thread_id();


// Platform implemented vscprintf
int p_vscprintf(const char * format, va_list pargs);
// Platform implemented scprintf
int p_scprintf(const char *format, ...);
// Platform implemented vsprintf_s
int p_vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list vlist);


// Platform sockets
int p_sock_init();

SocketHandle* p_sock_create_handle();
void p_sock_free_handle(SocketHandle* handle);

void p_sock_bind(SocketHandle* handle);
void p_sock_listen(SocketHandle* handle);
void p_sock_accept(SocketHandle* handle);
int p_sock_close(SocketHandle* handle);
void p_sock_shutdown(SocketHandle* handle);
int p_sock_send(SocketHandle* handle, char* buf, int len, int flags);
int p_sock_recv(SocketHandle* handle, char* buf, int len, int flags);


SocketHandle* p_setup_sockets_async(s32(*callback)(void*), void* params);
int p_sock_send_async(SocketHandle* handle, char* buf, int len, int flags);




// Creates thread, but does not start right away
ThreadHandle* p_thread_create_handle(u32(*thread_function)(void*), void* thread_args);

// Starts or resumes thread execution
void p_thread_resume(ThreadHandle* handle);

// This is called inside the thread function itself. The thread exits itself
// Most not be called by outside thread or processes
void p_thread_exit();

u32 p_thread_wait_for(ThreadHandle* handle, u32 milliseconds);

// Very dangerous, and should only be called in extreme cases
void p_thread_force_terminate(ThreadHandle* handle);

// Does not close the thread or exit thread. This should only be called once all those operations are properly done manually
// this function just frees internal memory holding onto the platform specific data
// To exit a thread, the thread itself should manually handle it
void p_thread_free_handle(ThreadHandle* handle);

u32 p_htonl(u32 hostlong);
u32 p_ntohl(u32 netlong);
u64 p_htonll(u64 hostlonglong);
u64 p_ntohll(u64 netlonglong);




JobQueue* p_create_job_queue();
bool p_try_push_job(JobQueue* queue, job_queue_callback* job_work, void* data);
void p_wait_for_job_completion(JobQueue* queue);
void p_free_job_queue(JobQueue* job_queue);


