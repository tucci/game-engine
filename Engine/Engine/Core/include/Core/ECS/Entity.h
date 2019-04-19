#pragma once

#define NO_ENTITY_ID (u64)0

#include "types.h"

struct Entity {
	// TODO: implement generation index
	u64 id;
	Entity() {
		id = NO_ENTITY_ID;
	}
	Entity(u64 entity_id) {
		id = entity_id;
	}
};

inline bool operator==(const Entity& lhs, const Entity& rhs) {
	return lhs.id == rhs.id;
}