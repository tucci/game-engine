#pragma once

#include "Common/LinearAllocator.h"
#include "Common/common_macros.h"

#include "debug_macros.h"



// NOTE: the passed in aligned size should be aligned by the caller
void linear_init(LinearAllocator* arena, void* start, size_t aligned_size) {
	//assert(start != NULL);
	arena->start = cast(char*) start;
	arena->end = arena->start + aligned_size;
	arena->current = cast(char*) start;
}

void* linear_alloc(LinearAllocator* arena, size_t size, size_t alignment) {
	size_t aligned_size = ALIGN_UP(size, alignment);
	// overrun debug check
	//assert(aligned_size < arena->end - arena->current);

	if (aligned_size > (size_t)(arena->end - arena->current)) {
		assert(aligned_size <= (size_t)(arena->end - arena->current));
		debug_print("Linear allocate overrun\n");
		return NULL;
	}

	void* ptr = arena->current;
	// Bump current ptr
	arena->current += aligned_size;
	return ptr;

	// Return where the current end of the mem is

}


void linear_reset(LinearAllocator* arena) {
	arena->current = arena->start;
}