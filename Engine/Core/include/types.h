#pragma once

#include <stdint.h>
#include <string.h>
#include "debug_macros.h"

// Typing _t everytime is ugly and annoying

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


// The string struct does not own the underlying string ptr
// Buf must be null terminated
struct IString {
	const size_t length;
	const char* const buf;
	// O(n) to init
	IString(const char* cstr) : buf(cstr), length(strlen(cstr)) {
	}
	// O(n) to init in debug mode, O(1) in non debug
	IString(const char* cstr, size_t len) : buf(cstr), length(len) {
#if DEBUG
		// In debug mode, make sure the length of the string passed in, is actually the same as the actual length of the string
		/*if (strlen(cstr) != len) {
			assert_fail();
		} */
#endif
		
	}
	
};