#pragma once


#include <stddef.h>

typedef struct StackHeader {
	size_t block_size;
} StackHeader;

typedef struct StackAllocator{
	char* start;
	char* end;
	char* current;
	char* last_reset_point;
	

} StackAllocator;

void stack_alloc_init(StackAllocator* sa, void* start, size_t aligned_size);
void* stack_alloc(StackAllocator* sa, size_t size, size_t alignment);
void stack_pop(StackAllocator* sa);

void stack_set_reset_point(StackAllocator* sa);

void stack_pop_to_last_reset_point(StackAllocator* sa);

void stack_reset(StackAllocator* sa);