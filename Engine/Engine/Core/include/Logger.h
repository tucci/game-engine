#pragma once

#include "debug_macros.h"
#include "types.h"

#include "Common/Arena.h"
#include "Common/StackAllocator.h"

#include <cstdarg>



enum class LoggerVerbosity: u32 {
	INFO,
	WARN,
	FATAL,
	VERBOSE,
};

struct LogItem {
	LoggerVerbosity verbosity;
	u64 time;
	s32 thread_id;
	s32 line;

	s32 tag_length;
	u64 function_length;
	u64 filename_length;
	u64 msg_length;

	const char* tag;
	const char* function;
	const char* filename;
	const char* msg;

};


struct Logger {
	LogItem* info;
	LogItem* warn;
	LogItem* fatal;
	LogItem* verbose;

	s32 info_count;
	s32 warn_count;
	s32 fatal_count;
	s32 verbose_count;

	
	FILE* log_file;
	bool inited;
	Arena logger_mem;
	StackAllocator stack;

	void(*log_callback)(void*);
	void* log_callback_data;
};



// Logger is global for now
void g_init_logger();
void g_destory_logger();
void g_attach_logger_callback(void(*log_callback)(void*), void* log_callback_data);
void g_detach_logger_callback();


void _log(
	const char* tag, s32 tag_length,
	LoggerVerbosity verbosity,
	const char* file, s32 file_str_size,
	const char* function_name, s32 function_str_size,
	s32 line,
	const char* msg,
	...);



#if DEBUG

#define LOG(log_type, tag, msg, ...) _log(tag, sizeof(tag), log_type, __FILE__, sizeof(__FILE__), __func__, sizeof(__func__), __LINE__, msg, __VA_ARGS__);
#define LOG_INFO(tag, msg, ...) LOG(LoggerVerbosity::INFO, tag, msg, __VA_ARGS__)
#define LOG_WARN(tag, msg, ...) LOG(LoggerVerbosity::WARN, tag, msg, __VA_ARGS__)
#define LOG_FATAL(tag, msg, ...) LOG(LoggerVerbosity::FATAL, tag, msg, __VA_ARGS__)
#define LOG_VERBOSE(tag, msg, ...) LOG(LoggerVerbosity::VERBOSE, tag, msg, __VA_ARGS__)


#else

// TODO: Not sure if logging should be in release??


#endif