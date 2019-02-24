#pragma once


#define DEBUG 1


#define NOOP do {} while(0)

#if DEBUG
	#include <stdio.h>
	#define debug_print(...) printf(__VA_ARGS__)

	#ifdef _MSC_VER
		#define DEBUG_BREAK __debugbreak()
	#else
	#endif

	

#else
	#define NDEBUG 
	#define debug_print(...) NOOP

	#ifdef _MSC_VER
		#define DEBUG_BREAK NOOP
	#else
	#endif
#endif



#include <assert.h>


#if DEBUG
	#define assert_fail() assert(0 && "Forced assert fail"); DEBUG_BREAK
#else
	#define assert_fail() assert(1)
#endif