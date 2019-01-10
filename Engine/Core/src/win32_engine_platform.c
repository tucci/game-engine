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

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef struct FileHandle {
	HANDLE win_handle;
} FileHandle;

typedef struct SocketHandle {
	SOCKET socket;
	struct addrinfo *result;
} SocketHandle;

typedef HWND WindowHandle;


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

int sock_init() {
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

SocketHandle sock_create() {
	SocketHandle handle; 
	handle.result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &handle.result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
	} else {
		handle.socket = INVALID_SOCKET;
		handle.socket = socket(handle.result->ai_family, handle.result->ai_socktype, handle.result->ai_protocol);

		if (handle.socket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(handle.result);
			WSACleanup();
		} else {


	}

}

	return handle;
}

void sock_bind(SocketHandle handle) {
	// Setup the TCP listening socket
	int iResult = bind(handle.socket, handle.result->ai_addr, (int)handle.result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(handle.result);
		closesocket(handle.socket);
		WSACleanup();
	} else {
		freeaddrinfo(handle.result);
	}
}

void sock_listen(SocketHandle handle) {
	if (listen(handle.socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(handle.socket);
		WSACleanup();	
	} else {
	}
}

void sock_accept(SocketHandle handle) {
	SocketHandle client_handle;
	client_handle.socket = INVALID_SOCKET;
	// Accept a client socket
	
	client_handle.socket = accept(handle.socket, NULL, NULL);
	if (client_handle.socket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(handle.socket);
		WSACleanup();
	}
}
int sock_close(SocketHandle handle) {
	return closesocket(handle.socket);
}

void sock_shutdown(SocketHandle handle) {
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(handle.socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
	}
	// cleanup
	closesocket(handle.socket);
	WSACleanup();
}
int sock_send(SocketHandle handle, char* buf, int len, int flags) {
	// Echo the buffer back to the sender
	int iSendResult = send(handle.socket, buf, len, flags);
	printf("Bytes sent: %d\n", iSendResult);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(handle.socket);
		WSACleanup();
		iSendResult = 0;
	}
	

	return iSendResult;
}
int sock_recv(SocketHandle handle, char* buf, int len, int flags) {
	int iResult = recv(handle.socket, buf, len, flags);
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);
		return iResult;
	} else if (iResult == 0) {
		printf("Connection closing...\n");
		return 0;
	} else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(handle.socket);
		WSACleanup();
		return -1;
	}
}


//void socket_stuff() {
//	WSADATA wsaData;
//	int iResult;
//
//	SOCKET ListenSocket = INVALID_SOCKET;
//	SOCKET ClientSocket = INVALID_SOCKET;
//
//	struct addrinfo *result = NULL;
//	struct addrinfo hints;
//
//	int iSendResult;
//	char recvbuf[DEFAULT_BUFLEN];
//	int recvbuflen = DEFAULT_BUFLEN;
//
//	// Initialize Winsock
//	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != 0) {
//		printf("WSAStartup failed with error: %d\n", iResult);
//		return 1;
//	}
//
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//	hints.ai_flags = AI_PASSIVE;
//
//	// Resolve the server address and port
//	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
//	if (iResult != 0) {
//		printf("getaddrinfo failed with error: %d\n", iResult);
//		WSACleanup();
//		return 1;
//	}
//
//	// Create a SOCKET for connecting to server
//	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//	if (ListenSocket == INVALID_SOCKET) {
//		printf("socket failed with error: %ld\n", WSAGetLastError());
//		freeaddrinfo(result);
//		WSACleanup();
//		return 1;
//	}
//
//	// Setup the TCP listening socket
//	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
//	if (iResult == SOCKET_ERROR) {
//		printf("bind failed with error: %d\n", WSAGetLastError());
//		freeaddrinfo(result);
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	freeaddrinfo(result);
//
//	iResult = listen(ListenSocket, SOMAXCONN);
//	if (iResult == SOCKET_ERROR) {
//		printf("listen failed with error: %d\n", WSAGetLastError());
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	// Accept a client socket
//	ClientSocket = accept(ListenSocket, NULL, NULL);
//	if (ClientSocket == INVALID_SOCKET) {
//		printf("accept failed with error: %d\n", WSAGetLastError());
//		closesocket(ListenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	// No longer need server socket
//	closesocket(ListenSocket);
//
//	// Receive until the peer shuts down the connection
//	do {
//
//		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
//		if (iResult > 0) {
//			printf("Bytes received: %d\n", iResult);
//			printf(recvbuf);
//
//			
//			// Echo the buffer back to the sender
//			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
//			if (iSendResult == SOCKET_ERROR) {
//				printf("send failed with error: %d\n", WSAGetLastError());
//				closesocket(ClientSocket);
//				WSACleanup();
//				return 1;
//			}
//			printf("Bytes sent: %d\n", iSendResult);
//		} else if (iResult == 0)
//			printf("Connection closing...\n");
//		else {
//			printf("recv failed with error: %d\n", WSAGetLastError());
//			closesocket(ClientSocket);
//			WSACleanup();
//			return 1;
//		}
//
//	} while (iResult > 0);
//
//	// shutdown the connection since we're done
//	iResult = shutdown(ClientSocket, SD_SEND);
//	if (iResult == SOCKET_ERROR) {
//		printf("shutdown failed with error: %d\n", WSAGetLastError());
//		closesocket(ClientSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	// cleanup
//	closesocket(ClientSocket);
//	WSACleanup();
//}


#endif