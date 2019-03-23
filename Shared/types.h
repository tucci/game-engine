#pragma once

#include <stdint.h>
#include <string.h>


// Typing _t everytime is ugly and annoying
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


struct Buffer {
	size_t size;
	s8* buffer;
};

// The string struct does not own the underlying string ptr
// Buf must be null terminated
// This struct is just a wrapper around char* that also holds the length of the c string
struct String {
	size_t length;
	const char* buffer;
	String(const char* cstr) : buffer(cstr), length(strlen(cstr)) {}
	String(const char* cstr, size_t len) : buffer(cstr), length(len) {}
};


