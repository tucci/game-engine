#pragma once

#define NO_ENTITY_ID (u64)0

#include "types.h"

struct Entity {
	// TODO: implement generation index
	u64 id;
	Entity() {
		id = NO_ENTITY_ID;
	}
};