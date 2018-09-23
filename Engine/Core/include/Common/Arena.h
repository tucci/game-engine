#pragma once



#define ARENA_ALIGNMENT 8
#define ARENA_BLOCK_SIZE (1024 * 1024)

typedef struct Arena {
	char *ptr;
	char *end;
	char **blocks;
	Arena() {
		ptr = 0;
		end = 0;
		blocks = 0;
	}
} Arena;


void arena_init(Arena* arena);

void arena_grow(Arena *arena, size_t min_size);

void *arena_alloc(Arena *arena, size_t size);

void arena_free(Arena *arena);