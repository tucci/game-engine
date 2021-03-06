#pragma once

#include "Common/StackAllocator.h"
#include "Common/common_macros.h"
#include "debug_macros.h"

void stack_alloc_init(StackAllocator* sa, void* start, size_t aligned_size) {
	sa->start = cast(char*) start;
	sa->end = sa->start + aligned_size;
	sa->current = sa->start;
	sa->last_reset_point = sa->start;
}

void* stack_alloc(StackAllocator* sa, size_t size, size_t alignment) {
	// TODO: need to figure out how to have smaller alignment due to the header increasing the block size insanely
	size_t aligned_size = ALIGN_UP(size + sizeof(StackHeader), alignment);
	// TODO: only do this in debug mode
	if (aligned_size > (size_t)(sa->end - sa->current)) {
		assert(false && "Stack Allocate overrun");
		debug_print("stack allocate overrun\n");
		return NULL;
	}

	// NOTE: the header is at the end of the block
	StackHeader* header = cast(StackHeader*)(sa->current + aligned_size - sizeof(StackHeader));
	header->block_size = aligned_size - sizeof(StackHeader);
	void* ptr = cast(void*)sa->current;
	sa->current += aligned_size;
	return ptr;

}

// TODO: wherever stack pop is called, we should move it to use a defer macro where we are allocating
void stack_pop(StackAllocator* sa) {
	// If there is nothing on the stack. do nothing
	if (sa->start == sa->current) return;
	StackHeader* header = cast(StackHeader*)(sa->current - sizeof(StackHeader));
	sa->current -= header->block_size + sizeof(StackHeader);
}

void stack_set_reset_point(StackAllocator* sa) {
	sa->last_reset_point = sa->current;
}

void stack_pop_to_last_reset_point(StackAllocator* sa) {
	sa->current = sa->last_reset_point;
	sa->last_reset_point = sa->start;
}


void stack_reset(StackAllocator* sa) {
	sa->current = sa->start;
	sa->last_reset_point = sa->start;
}