#pragma once

#include "Logger.h"
#include "Common/stretchy_buffer.h"
#include "engine_platform.h"
#include "Common/common_macros.h"
#include <time.h>

static Logger _logger;


void _init_logger() {

	arena_init(&_logger.logger_mem);
	size_t stack_size = MEGABYTES(1);
	void* stack_start = arena_alloc(&_logger.logger_mem, stack_size);
	stack_alloc_init(&_logger.stack, stack_start, stack_size);

	_logger.info = NULL;
	_logger.warn = NULL;
	_logger.fatal = NULL;
	_logger.verbose = NULL;

	_logger.info_count = 0;
	_logger.warn_count = 0;
	_logger.fatal_count = 0;
	_logger.verbose_count = 0;
	_logger.inited = true;

	
	errno_t err;
	const char* log_file_str = "log.txt";
	err = fopen_s(&_logger.log_file, log_file_str, "w");

	if (err == 0) {
		debug_print("Opened log file\n");
	} else {
		assert_fail();
		debug_print("Fail opening log file\n");
	}

}

void _destory_logger() {
	arena_free(&_logger.logger_mem);
	// This is not neccessary since the stack is using the arena as its memory store
	stack_reset(&_logger.stack);

	
	sb_free(_logger.info);
	sb_free(_logger.warn);
	sb_free(_logger.fatal);
	sb_free(_logger.verbose);

	_logger.info_count = 0;
	_logger.warn_count = 0;
	_logger.fatal_count = 0;
	_logger.verbose_count = 0;

	errno_t err;
	err = fclose(_logger.log_file);
	if (err == 0) {
		debug_print("Finished writing to log");
	} else {
		assert_fail();
		debug_print("Cannot close log file\n");
	}
}

// We will copy the msg into our memory
static const char* _log_cpy_str(const char* str, s32 msg_length) {
	size_t str_size = msg_length + 1;
	char* str_cpy = (char*)arena_alloc(&_logger.logger_mem, str_size);
	memcpy(str_cpy, str, str_size);
	return str_cpy;
}

void _log(s32 tag, LoggerVerbosity verbosity,
	const char* file, s32 file_str_size,
	const char* function_name, s32 function_str_size,
	s32 line,
	const char* msg,
	...) {





	va_list args;
	va_start(args, msg);

	size_t formatted_msg_size = p_vscprintf(msg, args) + 1;
	char* formatted_msg_buf = (char*)arena_alloc(&_logger.logger_mem, formatted_msg_size);
	
	p_vsprintf_s(formatted_msg_buf, formatted_msg_size, msg, args);
	va_end(args);




	// Copy strings
	const char* file_cpy = _log_cpy_str(file, file_str_size);
	const char* function_name_cpy = _log_cpy_str(function_name, function_str_size);


	s64 tid = platform_get_thread_id();



	// Time 
	time_t ltime = time(NULL);
	u64 time = (u64) ltime;

	LogItem item = {
		time,
		tag,
		tid,
		line,

		function_str_size,
		file_str_size,
		formatted_msg_size,

		function_name_cpy,
		file_cpy,
		formatted_msg_buf,
	};



	
	const char* log_type_str;
	switch (verbosity) {
		case VERBOSITY_INFO: {
			sb_push(_logger.info, item);
			_logger.info_count++;
			log_type_str = "[INFO]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}

		case VERBOSITY_WARN: {
			sb_push(_logger.warn, item);
			_logger.warn_count++;
			log_type_str = "[WARN]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}
		case VERBOSITY_FATAL: {
			sb_push(_logger.fatal, item);
			_logger.fatal_count++;
			log_type_str = "[FATAL]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}
		case VERBOSITY_VERBOSE: {
			sb_push(_logger.verbose, item);
			_logger.verbose_count++;
			log_type_str = "[VERBOSE]";
			//debug_print("%s %s\n", "[VERBOSE]", formatted_msg_buf);
			break;
		}
		default: {
			log_type_str = "[NONE]";
			break;
		}
	}
	
	// Getting time string
	tm timeinfo;
	localtime_s(&timeinfo, &ltime);
	char time_buf[70];
	strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);
	
	const char* fmt = "%s DATE:%s, TAG:%d, TID:%d, FILE:%s, FUNC:%s, LINE:%d, MSG: %s";
	

	// Getting full log string output and size
	int full_log_str_size = p_scprintf(fmt, log_type_str, time_buf, item.tag, item.thread_id, item.filename, item.function, item.line, item.msg) + 1;
	// alloc space on stack for our log output
	char* full_log_str = (char*)stack_alloc(&_logger.stack, full_log_str_size, 1);// + 1 for terminator
	// Print string to buffer
	// TODO: see if this is portable or needs to be platform specific
	snprintf(full_log_str, full_log_str_size, fmt, log_type_str, time_buf, item.tag, item.thread_id, item.filename, item.function, item.line, item.msg);
	// write string to file
	fwrite(full_log_str, full_log_str_size, 1, _logger.log_file);
	fwrite("\n", sizeof(char), 1, _logger.log_file);
	// Free the string for next log
	stack_pop(&_logger.stack);

	
}