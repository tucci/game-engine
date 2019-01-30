#pragma once

#include "Common/Arena.h"
#include "Common/StackAllocator.h"
#include "types.h"
#include <cstdarg>


enum class LoggerVerbosity {
	INFO,
	WARN,
	FATAL,
	VERBOSE,
};

struct LogItem {
	u64 time;
	s32 tag;

	s32 thread_id;

	s32 line;

	s32 function_length;
	s32 filename_length;
	s32 msg_length;

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
	
};



void _init_logger();
void _destory_logger();



void _log(
	s32 tag,
	LoggerVerbosity verbosity,
	const char* file, s32 file_str_size,
	const char* function_name, s32 function_str_size,
	s32 line,
	const char* msg,
	...);



#if DEBUG

#define LOG(log_type, tag, msg, ...) _log(tag, log_type, __FILE__, sizeof(__FILE__), __func__, sizeof(__func__), __LINE__, msg, __VA_ARGS__);
#define LOG_INFO(tag, msg, ...) LOG(LoggerVerbosity::INFO, tag, msg, __VA_ARGS__)
#define LOG_WARN(tag, msg, ...) LOG(LoggerVerbosity::WARN, tag, msg, __VA_ARGS__)
#define LOG_FATAL(tag, msg, ...) LOG(LoggerVerbosity::FATAL, tag, msg, __VA_ARGS__)
#define LOG_VERBOSE(tag, msg, ...) LOG(LoggerVerbosity::VERBOSE, tag, msg, __VA_ARGS__)


#else

// TODO: Not sure if logging should be in release??


#endif