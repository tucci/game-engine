#pragma once

#include "Logger.h"
#include "Common/stretchy_buffer.h"
#include "engine_platform.h"
#include "Common/common_macros.h"
#include <time.h>

static Logger g_logger;


void g_init_logger() {

	arena_init(&g_logger.logger_mem);
	size_t stack_size = MEGABYTES(1);
	void* stack_start = arena_alloc(&g_logger.logger_mem, stack_size);
	stack_alloc_init(&g_logger.stack, stack_start, stack_size);

	g_logger.logs = NULL;
	g_logger.log_count = 0;
	//g_logger.info = NULL;
	//g_logger.warn = NULL;
	//g_logger.fatal = NULL;
	//g_logger.verbose = NULL;
	//
	//g_logger.info_count = 0;
	//g_logger.warn_count = 0;
	//g_logger.fatal_count = 0;
	//g_logger.verbose_count = 0;
	g_logger.inited = true;
	g_logger.log_callback = NULL;

	
	errno_t err;
	const char* log_file_str = "log.txt";
	err = fopen_s(&g_logger.log_file, log_file_str, "w");

	if (err == 0) {
		debug_print("Opened log file\n");
	} else {
		assert_fail();
		debug_print("Fail opening log file\n");
	}

}

void g_destory_logger() {
	arena_free(&g_logger.logger_mem);
	// This is not neccessary since the stack is using the arena as its memory store
	stack_reset(&g_logger.stack);

	sb_free(g_logger.logs);
	g_logger.log_count = 0;
	//sb_free(g_logger.info);
	//sb_free(g_logger.warn);
	//sb_free(g_logger.fatal);
	//sb_free(g_logger.verbose);

	//g_logger.info_count = 0;
	//g_logger.warn_count = 0;
	//g_logger.fatal_count = 0;
	//g_logger.verbose_count = 0;

	errno_t err;
	err = fclose(g_logger.log_file);
	if (err == 0) {
		debug_print("Finished writing to log");
	} else {
		assert_fail();
		debug_print("Cannot close log file\n");
	}
}

void g_attach_logger_callback(void(*log_callback)(void*), void* callback_data) {
	g_logger.log_callback = log_callback;
	g_logger.log_callback_data = callback_data;
}

void g_detach_logger_callback() {
	g_logger.log_callback = NULL;
	g_logger.log_callback_data = NULL;
}


// We will copy the msg into our memory
static const char* _log_cpy_str(const char* str, s32 msg_length) {
	size_t str_size = msg_length + 1;
	char* str_cpy = (char*)arena_alloc(&g_logger.logger_mem, str_size);
	memcpy(str_cpy, str, str_size);
	return str_cpy;
}

void _log(const char* tag,
	s32 tag_str_size,
	LoggerVerbosity verbosity,
	const char* file, s32 file_str_size,
	const char* function_name, s32 function_str_size,
	s32 line,
	const char* msg,
	...) {





	va_list args;
	va_start(args, msg);

	size_t formatted_msg_size = p_vscprintf(msg, args) + 1;
	char* formatted_msg_buf = (char*)arena_alloc(&g_logger.logger_mem, formatted_msg_size);
	
	p_vsprintf_s(formatted_msg_buf, formatted_msg_size, msg, args);
	va_end(args);




	const char* tag_cpy;
	// Prevent null pointer string
	if (tag == 0) {
		tag_cpy = _log_cpy_str("NONE", 4);
	} else {
		tag_cpy = _log_cpy_str(tag, tag_str_size);
	}
	// Copy strings
	
	const char* file_cpy = _log_cpy_str(file, file_str_size);
	const char* function_name_cpy = _log_cpy_str(function_name, function_str_size);


	s64 tid = platform_get_thread_id();



	// hpc time
	u64 ms_time = platform_ms_since_epoch();

	LogItem item = {
		verbosity,
		ms_time,
		
		tid,
		line,

		tag_str_size,
		function_str_size,
		file_str_size,
		formatted_msg_size,

		tag_cpy,
		function_name_cpy,
		file_cpy,
		formatted_msg_buf,
	};



	
	const char* log_type_str;
	sb_push(g_logger.logs, item);
	g_logger.log_count++;

	switch (verbosity) {
		case LoggerVerbosity::INFO: {
			//sb_push(g_logger.info, item);
			//g_logger.info_count++;
			log_type_str = "[INFO]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}

		case LoggerVerbosity::WARN: {
			//sb_push(g_logger.warn, item);
			//g_logger.warn_count++;
			log_type_str = "[WARN]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}
		case LoggerVerbosity::FATAL: {
			//sb_push(g_logger.fatal, item);
			//g_logger.fatal_count++;
			log_type_str = "[FATAL]";
			debug_print("%s %s\n", log_type_str, formatted_msg_buf);
			break;
		}
		case LoggerVerbosity::VERBOSE: {
			//sb_push(g_logger.verbose, item);
			//g_logger.verbose_count++;
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
	// lower res than the time we passed to the log item
	time_t ltime = time(NULL);
	tm timeinfo;
	localtime_s(&timeinfo, &ltime);
	char time_buf[70];
	strftime(time_buf, sizeof(time_buf), "%c", &timeinfo);
	
	const char* fmt = "%s DATE:%s, TAG:%s, TID:%d, FILE:%s, FUNC:%s, LINE:%d, MSG: %s";
	

	// Getting full log string output and size
	int full_log_str_size = p_scprintf(fmt, log_type_str, time_buf, item.tag, item.thread_id, item.filename, item.function, item.line, item.msg) + 1;
	// alloc space on stack for our log output
	char* full_log_str = (char*)stack_alloc(&g_logger.stack, full_log_str_size, 1);// + 1 for terminator
	// Print string to buffer
	// TODO: see if this is portable or needs to be platform specific
	snprintf(full_log_str, full_log_str_size, fmt, log_type_str, time_buf, item.tag, item.thread_id, item.filename, item.function, item.line, item.msg);
	// write string to file
	fwrite(full_log_str, full_log_str_size, 1, g_logger.log_file);
	fwrite("\n", sizeof(char), 1, g_logger.log_file);
	 
	
	// Callback is used to send a log item to some other subsystem to parse
	// Currently this is being used to send a log item to the editor to display
	if (g_logger.log_callback != NULL) {
		// Since the g_attach_logger_callback allows us to a pass a void* data, and we need to also pass the log item
		// we need to merge the params into one param struct
		// the param struct has the given callback data given to us when g_attach_logger_callback is called
		// and the log item that has just been emitted
		
		struct {
			void* given_callback_data;// the g_attach_logger_callback data passed in
			LogItem* item; // the log item
		} merge_param;


		merge_param.given_callback_data = g_logger.log_callback_data;
		merge_param.item = &item;
		g_logger.log_callback((void*)&merge_param);
	}
	// Free the string for next log
	stack_pop(&g_logger.stack);

	
}

LogList g_get_loglist() {
	LogList list;
	list.logs = g_logger.logs;
	list.log_count = g_logger.log_count;
	return list;
}