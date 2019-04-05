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

inline bool operator==(const Entity& lhs, const Entity& rhs) {
	return lhs.id == rhs.id;
}