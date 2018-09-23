#pragma once

#include <stdint.h.>


typedef struct Map {
	uint64_t* key;
	uint64_t* value;
	size_t len;
	size_t cap;
} Map;


void* map_get(Map* map, const void *key);
void map_put(Map* map, const void *key, void* val);