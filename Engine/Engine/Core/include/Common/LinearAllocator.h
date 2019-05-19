#pragma once

#include <stddef.h>

struct LinearAllocator {
	char* start;
	char* end;
	char* current;
};

void linear_init(LinearAllocator* arena, void* start, size_t aligned_size);
void* linear_alloc(LinearAllocator* arena, size_t size, size_t alignment);
void linear_reset(LinearAllocator* arena);