#pragma once


#include <stddef.h>

typedef struct StackHeader {
	size_t block_size;
} StackHeader;

typedef struct StackAllocator{
	char* start;
	char* end;
	char* current;
} StackAllocator;

// TODO: the stack allocator wastes a lot of space becaus of the header alignment issue. see if we can fix this without causing too much wasted space
void stack_alloc_init(StackAllocator* sa, void* start, size_t aligned_size);
void* stack_alloc(StackAllocator* sa, size_t size, size_t alignment);
void stack_pop(StackAllocator* sa);