#pragma once

#include <stddef.h>

typedef struct LinearAllocator {
	char* start;
	char* end;
	char* current;
} LinearAllocator;

void linear_init(LinearAllocator* arena, void* start, size_t aligned_size);
void* linear_alloc(LinearAllocator* arena, size_t size, size_t alignment);
void linear_reset(LinearAllocator* arena);