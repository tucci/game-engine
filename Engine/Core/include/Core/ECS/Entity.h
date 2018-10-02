#pragma once

#define NO_ENTITY_ID (uint64_t)0

#include <stdint.h>

typedef struct Entity {
	// TODO: implement generation index
	uint64_t id;
	Entity() {
		id = NO_ENTITY_ID;
	}
} Entity;