#pragma once

#ifdef _WIN32

#include "engine_platform.h"


#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Shlwapi.h"
#include <limits.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")


#define DEFAULT_PORT 27016
#define DATA_BUFSIZE 8192
struct FileHandle {
	HANDLE win_handle;
};

struct ThreadHandle {
	HANDLE thread_handle;
	DWORD win_thread_id;
};



struct IOCP_DATA {
	OVERLAPPED Overlapped;
	HANDLE ioport_handle;
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
	CHAR Buffer[DATA_BUFSIZE];
};

struct WorkerThreadParams {
	HANDLE completion_port;
	s32(*callback)(void*);
	void* callback_params;
};

struct SocketHandle {
	

	SOCKET listen_socket;
	SOCKET accept_socket;

	WSAEVENT accept_event;
	//struct addrinfo *result;

	IOCP_DATA iocp;
	WorkerThreadParams worker_thread;

	
};

typedef HWND WindowHandle;



struct JobThreadParam {
	int thread_index;
	struct JobQueue* job_queue;
};


struct JobEntry {
	job_queue_callback* callback;
	void* data;
};


#define THREAD_COUNT 8
#define WORK_QUEUE_SIZE 256

struct JobQueue {

	HANDLE semaphore;
	volatile s32 completion_goal;
	volatile s32 completion_count;

	volatile s32 next_entry_to_write;
	volatile s32 next_entry_to_read;

	s32 job_added_succeeded;
	s32 job_added_failed;

	JobEntry job_queue[WORK_QUEUE_SIZE];
	JobThreadParam thread_params[THREAD_COUNT];
};

static DWORD WINAPI JobThreadProc(LPVOID lpParam) {
	JobThreadParam* tparam = (JobThreadParam*)lpParam;
	JobQueue* job_queue = tparam->job_queue;
	for (;;) {

		s32 original_next_entry_to_read = job_queue->next_entry_to_read;
		s32 new_next_entry_to_read = (original_next_entry_to_read + 1) % WORK_QUEUE_SIZE;

		if (original_next_entry_to_read != job_queue->next_entry_to_write) {
			s32 entry_index = InterlockedCompareExchange((LONG* volatile)&job_queue->next_entry_to_read, new_next_entry_to_read, original_next_entry_to_read);

			if (entry_index == original_next_entry_to_read) {
				MemoryBarrier();
				
				JobEntry* job = &job_queue->job_queue[entry_index];

				job->callback(job->data);

				InterlockedIncrement((LONG* volatile)&job_queue->completion_count);
				//Sleep(100);
			}


		} else {
			WaitForSingleObject(job_queue->semaphore, INFINITE);
		}
	}

	return 0;
}



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




FileHandle* platform_create_filehandle(const char* filename) {
	assert(filename != NULL);
	FileHandle* fh = (FileHandle*)malloc(sizeof(FileHandle));

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
		fh->win_handle = h;
	} else {
		// error
		fh->win_handle = NULL;
	}


	return fh;
}

void platform_free_filehandle(FileHandle* handle) {
	free(handle);
}


bool platform_close_file(FileHandle* handle) {
	return CloseHandle(handle->win_handle);
}

u32 platform_path(FileHandle* handle, char* path, u32 path_length) {
	assert(path != NULL);

	DWORD length_of_actual_path = GetFinalPathNameByHandle(
		handle->win_handle,
		path,
		path_length,
		VOLUME_NAME_NONE
	);
	return length_of_actual_path;
}

s64 platform_get_file_size(FileHandle* handle) {
	LARGE_INTEGER size;
	if (GetFileSizeEx(handle->win_handle, &size)) {
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

s64 platform_ms_since_epoch() {
	// Modified from https://git.postgresql.org/gitweb/?p=postgresql.git;a=blob;f=src/port/gettimeofday.c;h=75a91993b74414c0a1c13a2a09ce739cb8aa8a08;hb=HEAD


	timeval tp = { 0 };

	FILETIME file_time;
	SYSTEMTIME system_time;
	ULARGE_INTEGER ularge;
	
	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;
	
	tp.tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp.tv_usec = (long)(system_time.wMilliseconds * 1000);

	s64 seconds_to_ms = (s64)tp.tv_sec * 1000;
	// There should be no overflow since LONG_MAX * 1000 < _I64_MAX
	// but safe t check
	assert(seconds_to_ms > 0);
	assert(seconds_to_ms < _I64_MAX);

	// There also should be no overflow
	s64 nano_to_ms = tp.tv_usec / 1000;
	assert(nano_to_ms > 0);
	assert(nano_to_ms < _I64_MAX);

	// There may be overlfow here since s64 + s64 may be > _I64_MAX
	// however since we are dealing with time, seconds_to_ms will be significantly lower than _I64_MAX
	// and nano_to_ms is very small
	s64 ms = seconds_to_ms + nano_to_ms;

	assert(ms > 0);
	assert(ms < _I64_MAX);
	return ms;
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

int p_sock_init() {
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		//printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	return iResult;
}

SocketHandle* p_sock_create_handle() {
	SocketHandle* handle = (SocketHandle*)malloc(sizeof(SocketHandle));

	handle->listen_socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (handle->listen_socket == INVALID_SOCKET) {
		WSACleanup();
	}

	//handle->result = NULL;
	//struct addrinfo *ptr = NULL;
	//struct addrinfo hints;
	//
	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;
	//
	//// Resolve the local address and port to be used by the server
	//int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &handle->result);
	//if (iResult != 0) {
	//	printf("getaddrinfo failed: %d\n", iResult);
	//	WSACleanup();
	//} else {
	//	handle->socket = INVALID_SOCKET;
	//	handle->socket = socket(handle->result->ai_family, handle->result->ai_socktype, handle->result->ai_protocol);
	//
	//	if (handle->socket == INVALID_SOCKET) {
	//		printf("socket failed with error: %ld\n", WSAGetLastError());
	//		freeaddrinfo(handle->result);
	//		WSACleanup();
	//	} else {
	//
	//
	//}

//}
	


	return handle;
}

void p_sock_bind(SocketHandle* handle) {
	// TODO: change address to use localhost
	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(DEFAULT_PORT);

	// Setup the TCP listening socket
	//int iResult = bind(handle->socket, handle->result->ai_addr, (int)handle->result->ai_addrlen);
	int iResult = bind(handle->listen_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		//freeaddrinfo(handle->result);
		closesocket(handle->listen_socket);
		WSACleanup();
	} else {
		//freeaddrinfo(handle->result);
	}
}

void p_sock_listen(SocketHandle* handle) {
	if (listen(handle->listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(handle->listen_socket);
		WSACleanup();	
	} else {

	}
}


// Accept is blocking
void p_sock_accept(SocketHandle* handle) {
	SOCKET client;
	client = INVALID_SOCKET;
	// Accept a client socket
	/*handle->accept_event = WSACreateEvent();
	if (handle->accept_event == WSA_INVALID_EVENT) {
		WSACleanup();
	}*/

	while (true) {
		
		handle->accept_socket = accept(handle->listen_socket, NULL, NULL);
		closesocket(handle->listen_socket);
		if (WSASetEvent(handle->accept_event) == FALSE) {
			WSACleanup();
		} else {
			//// Update the current socket with the client socket
			//handle->listen_socket = client;
			break;
		}
	}
	//client = accept(handle->socket, NULL, NULL);
	//if (client == INVALID_SOCKET) {
	//	printf("accept failed: %d\n", WSAGetLastError());
	//	closesocket(handle->socket);
	//	WSACleanup();
	//}
	//closesocket(handle->socket);
	//// Update the current socket with the client socket
	//handle->socket = client;
}
int p_sock_close(SocketHandle* handle) {
	return closesocket(handle->listen_socket);
}

void p_sock_free_handle(SocketHandle* handle) {
	free(handle);
}

void p_sock_shutdown(SocketHandle* handle) {
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(handle->accept_socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
	}
	// cleanup
	closesocket(handle->accept_socket);
	WSACleanup();
}

int p_sock_send(SocketHandle* handle, char* buf, int len, int flags) {
	
	// Echo the buffer back to the sender
	int iSendResult = send(handle->accept_socket, buf, len, flags);
	printf("Bytes sent: %d\n", iSendResult);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(handle->accept_socket);
		WSACleanup();
		iSendResult = 0;
	}
	

	return iSendResult;
}
int p_sock_recv(SocketHandle* handle, char* buf, int len, int flags) {
	int iResult = recv(handle->accept_socket, buf, len, flags);
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);
		return iResult;
	} else if (iResult == 0) {
		printf("Connection closing...\n");
		return 0;
	} else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(handle->accept_socket);
		WSACleanup();
		return -1;
	}
}

u32 p_htonl(u32 hostlong) {
	return htonl(hostlong);
}

u32 p_ntohl(u32 netlong) {
	return ntohl(netlong);
}

u64 p_htonll(u64 hostlonglong) {
	return htonll(hostlonglong);
}

u64 p_ntohll(u64 netlonglong) {
	return ntohll(netlonglong);
}

ThreadHandle* p_thread_create_handle(u32 (*thread_function)(void*), void* thread_args) {
	ThreadHandle* handle = (ThreadHandle*)malloc(sizeof(ThreadHandle));
	u32 creation_flags = CREATE_SUSPENDED;
	handle->thread_handle = CreateThread(NULL, 0,  (LPTHREAD_START_ROUTINE)thread_function, (LPVOID)thread_args, creation_flags, &handle->win_thread_id);
	return handle;
}

void p_thread_resume(ThreadHandle* handle) {
	ResumeThread(handle->thread_handle);
}

void p_thread_exit() {
	ExitThread(0);
}

u32 p_thread_wait_for(ThreadHandle* handle, u32 milliseconds) {
	DWORD code = WaitForSingleObject(handle->thread_handle, (DWORD)milliseconds);
	// TODO: convert ms specific return codes to a more generic virtual codes
	return (u32)code;
}

void p_thread_force_terminate(ThreadHandle* handle) {
	bool code = TerminateThread(handle->thread_handle, 1);
	CloseHandle(handle->thread_handle);
}

void p_thread_free_handle(ThreadHandle* handle) {
	free(handle);
}


static DWORD WINAPI ServerWorkerThread(LPVOID params) {
	// taken and modified from https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5i.html

	

	WorkerThreadParams* p;
	p = (WorkerThreadParams*)params;

	
	
	
	DWORD BytesTransferred;
	SocketHandle* socket_handle;
	IOCP_DATA* PerIoData;
	DWORD SendBytes, RecvBytes;
	DWORD Flags;
	while (TRUE) {
		if (GetQueuedCompletionStatus(p->completion_port, &BytesTransferred, (PULONG_PTR)&socket_handle, (LPOVERLAPPED*)&PerIoData, INFINITE) == 0) {
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			return 0;
		}
		// First check to see if an error has occurred on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket
		if (BytesTransferred == 0) {
			printf("Closing socket\n");
			if (closesocket(socket_handle->accept_socket) == SOCKET_ERROR) {
				printf("closesocket() failed with error %d\n", WSAGetLastError());
				return 0;
			}
			continue;
		}
		// Check to see if the BytesRECV field equals zero. If this is so, then
		// this means a WSARecv call just completed so update the BytesRECV field
		// with the BytesTransferred value from the completed WSARecv() call

		if (PerIoData->BytesRECV == 0) {
			PerIoData->BytesRECV = BytesTransferred;
			PerIoData->BytesSEND = 0;
		} else {
			PerIoData->BytesSEND += BytesTransferred;
		}

		if (PerIoData->BytesRECV > PerIoData->BytesSEND) {
			// Post another WSASend() request.
			// Since WSASend() is not guaranteed to send all of the bytes requested,
			// continue posting WSASend() calls until all received bytes are sent.
			ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
			PerIoData->DataBuf.buf = PerIoData->Buffer + PerIoData->BytesSEND;
			PerIoData->DataBuf.len = PerIoData->BytesRECV - PerIoData->BytesSEND;

			struct {
				char* buf;
				size_t buf_len;
				void* params;
			} callback_params;

			callback_params.buf = PerIoData->DataBuf.buf;
			callback_params.buf_len = (size_t)PerIoData->DataBuf.len;
			callback_params.params = p->callback_params;

			p->callback((void*)&callback_params);

			if (WSASend(socket_handle->accept_socket, &(PerIoData->DataBuf), 1, &SendBytes, 0, &(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					printf("WSASend() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			} else {
				printf("WSASend() is OK!\n");
			}
		} else {
			PerIoData->BytesRECV = 0;
			// Now that there are no more bytes to send, post another WSARecv() request
			Flags = 0;
			ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
			PerIoData->DataBuf.len = DATA_BUFSIZE;
			PerIoData->DataBuf.buf = PerIoData->Buffer;
			if (WSARecv(socket_handle->accept_socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags, &(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					printf("WSARecv() failed with error %d\n", WSAGetLastError());
					return 0;
				}
			} else {
				printf("WSARecv() is OK!\n");
			}
		}
	}
}

SocketHandle* p_setup_sockets_async(s32 (*callback)(void*), void* params) {
	int code = p_sock_init();


	HANDLE completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (completion_port == NULL) {
		return NULL;
	}
	
	// Get how many cores on this pc
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	int worker_thread_count = (int)sys_info.dwNumberOfProcessors;

	

	
	SocketHandle* socket_handle = p_sock_create_handle();

	


	
	socket_handle->worker_thread.completion_port = completion_port;
	socket_handle->worker_thread.callback = callback;
	socket_handle->worker_thread.callback_params = params;

	// Create the same amount of threads as we have cores on the pc
	DWORD thread_id;
	for (int i = 0; i < worker_thread_count; i++) {
		HANDLE thread_handle = CreateThread(NULL, 0, ServerWorkerThread, (void*)&socket_handle->worker_thread, 0, &thread_id);
		if (thread_handle == NULL) {
			return NULL;
		}
		// We wont need the handle, give it back to the os
		CloseHandle(thread_handle);
	}

	
	
	// Assign the completion port into the internal socket handle
	socket_handle->iocp.ioport_handle = completion_port;
	
	p_sock_bind(socket_handle);
	p_sock_listen(socket_handle);

	// Accept connections and assign to the completion port
	while (TRUE) {
		socket_handle->accept_socket = WSAAccept(socket_handle->listen_socket, NULL, NULL, NULL, 0);
		if (socket_handle->accept_socket == SOCKET_ERROR) {
			printf("WSAAccept() failed with error %d\n", WSAGetLastError());
			return NULL;
		} else {
			break;
		}
	}

	// once accepted, assign the new socket with the completion port
	CreateIoCompletionPort((HANDLE)socket_handle->accept_socket, completion_port, (ULONG_PTR)socket_handle, 0);

	ZeroMemory(&socket_handle->iocp.Overlapped, sizeof(socket_handle->iocp.Overlapped));
	socket_handle->iocp.BytesSEND = 0;
	socket_handle->iocp.BytesRECV = 0;
	socket_handle->iocp.DataBuf.len = DATA_BUFSIZE;
	socket_handle->iocp.DataBuf.buf = socket_handle->iocp.Buffer;

	DWORD flags = 0;

	
	DWORD recvbytes;
	if (WSARecv(socket_handle->accept_socket, &(socket_handle->iocp.DataBuf), 1, &recvbytes, &flags, &(socket_handle->iocp.Overlapped), NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("WSARecv() failed with error %d\n", WSAGetLastError());
			return NULL;
		}
	}




	return socket_handle;
}

int p_sock_send_async(SocketHandle* handle, char* buf, int len, int flags) {
	// TODO: implement async sends with callbacks. the callback might need to do memory freeing

	//if (WSASend(handle->accept_socket, &(buf), 1, &len, 0, &(handle->iocp.Overlapped), NULL) == SOCKET_ERROR) {
	//	if (WSAGetLastError() != ERROR_IO_PENDING) {
	//		printf("WSASend() failed with error %d\n", WSAGetLastError());
	//		return 0;
	//	}
	//} else {
	//	printf("WSASend() is OK!\n");
	//}
	return 0;
}


JobQueue* p_create_job_queue() {

	JobQueue* queue = (JobQueue*)malloc(sizeof(JobQueue));
	queue->next_entry_to_read = 0;
	queue->next_entry_to_write = 0;
	queue->completion_count = 0;
	queue->completion_goal = 0;
	queue->job_added_succeeded = 0;
	queue->job_added_failed = 0;

	LONG inital_count = 0;
	LONG thread_count = THREAD_COUNT;
	HANDLE sem = CreateSemaphore(NULL, inital_count, thread_count, 0);

	void* param;
	for (LONG i = 0; i < thread_count; i++) {
		DWORD thread_id;
		JobThreadParam* param = &queue->thread_params[i];
		param->thread_index = i;
		param->job_queue = queue;
		HANDLE thread = CreateThread(NULL, 0, JobThreadProc, param, 0, &thread_id);
		// We won't need handles to these anymore, we use all the threads until exit
		CloseHandle(thread);
	}

	return queue;
}

bool p_try_push_job(JobQueue* queue, job_queue_callback* job_work, void* data) {
	s32 new_next_entry_to_write = (queue->next_entry_to_write + 1) % WORK_QUEUE_SIZE;
	//assert(new_next_entry_to_write != queue->next_entry_to_read);
	if (new_next_entry_to_write == queue->next_entry_to_read) {
		printf("job failed to add\n");
		queue->job_added_failed++;
		return false;
	}

	// TODO: use interlocked compare exchange for multi producers adding
	// i.e multiple threads can add work at the same time
	JobEntry* job_entry = &queue->job_queue[queue->next_entry_to_write];
	job_entry->callback = job_work;
	job_entry->data = data;
	MemoryBarrier();
	++queue->completion_goal;
	queue->next_entry_to_write = new_next_entry_to_write;
	ReleaseSemaphore(queue->semaphore, 1, 0);
	printf("job added\n");
	queue->job_added_succeeded++;
	return true;

}

void p_wait_for_job_completion(JobQueue* queue) {
	while (queue->completion_count != queue->completion_goal);
	queue->completion_count = 0;
	queue->completion_goal = 0;
}

void p_free_job_queue(JobQueue* job_queue) {
	CloseHandle(job_queue->semaphore);
	free(job_queue);
}


#endif