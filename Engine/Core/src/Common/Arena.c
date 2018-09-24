#pragma once

#include "Common/Arena.h"


#include <stdlib.h>

#include "debug_macros.h"
#include "Common/common_macros.h"

#include "Common/stretchy_buffer.h"

void arena_init(Arena* arena) {
	arena->ptr = 0;
	arena->end = 0;
	arena->blocks = 0;
}

void arena_grow(Arena* arena, size_t min_size) {
	
	size_t size = ALIGN_UP(CLAMP_MIN(min_size, ARENA_BLOCK_SIZE), ARENA_ALIGNMENT);
	arena->ptr = cast(char*) malloc(size);
	assert(arena->ptr == ALIGN_DOWN_PTR(arena->ptr, ARENA_ALIGNMENT));
	arena->end = arena->ptr + size;
	stb_sb_push(arena->blocks, arena->ptr);
}

void *arena_alloc(Arena* arena, size_t size) {
	if (size > (size_t)(arena->end - arena->ptr)) {
		arena_grow(arena, size);
		assert(size <= (size_t)(arena->end - arena->ptr));
	}
	void *ptr = arena->ptr;
	arena->ptr = cast(char*) ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
	assert(arena->ptr <= arena->end);
	assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));
	return ptr;
}

void arena_free(Arena* arena) {
	int count = stb_sb_count(arena->blocks);
	for (int i = 0; i < count; i++) {
		free(arena->blocks[i]);
	}
	stb_sb_free(arena->blocks);
}