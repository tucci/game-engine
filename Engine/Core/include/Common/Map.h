#pragma once

#include <stdint.h.>
#include "Math/Math.h"
#include "debug_macros.h"
// TODO: move our malloc to use our custom allocator?
#include <stdlib.h>

template <typename V>
struct CompactMapItem {
	uint64_t key;
	V value;
};


template <typename V>
struct CompactMap {
	CompactMapItem<V>* map;
	size_t item_count; // Amount of items hashed inside the map
	size_t size;// Note: map size should be a power of 2
	uint64_t empty_ref = 0;
	//V& operator[] (uint64_t index);
	CompactMap() {
		//this->map = NULL;
		//this->size = 0;
		//this->item_count = 0;
	}
};
//
//template <typename V>
//void map_init(CompactMap<V>* map);
//
//template <typename V>
//void map_destroy(CompactMap<V>* map);
//
//// Can also use the [] operator
//template <typename V>
//V& map_get(CompactMap<V>* map, uint64_t key);
//
//template <typename V>
//void map_put(CompactMap<V>* map, uint64_t key, V val);
//
//template <typename V>
//void map_grow(CompactMap<V>* map, size_t new_size);
//
//static uint64_t MurmurHash3Mixer(uint64_t key);
//

static uint64_t MurmurHash3Mixer(uint64_t key) {
	key ^= (key >> 33);
	key *= 0xff51afd7ed558ccd;
	key ^= (key >> 33);
	key *= 0xc4ceb9fe1a85ec53;
	key ^= (key >> 33);

	return key;
}

template <typename V>
void map_init(CompactMap<V>* map) {
	map->map = NULL;
	map->size = 0;
	map->item_count = 0;
}

template <typename V>
void map_destroy(CompactMap<V>* map) {
	free(map->map);

}

template <typename V>
V map_get(CompactMap<V>* map, uint64_t key) {
	assert(key != 0);
	assert(IS_POW2(map->size));


	size_t hash_index = MurmurHash3Mixer(key);
	for (;;) {
		// Since our map size is a power of 2, we can mod it using this bit trick
		// same has hash = hash % map->size;
		hash_index &= (map->size - 1);
		if (map->map[hash_index].key == key) {
			return map->map[hash_index].value;
		} else if (map->map[hash_index].key == 0) {
			// Found an empty slot, meaning this key is not here
			return map->empty_ref;
		}
		hash_index++;
	}
	return map->empty_ref;

}

template <typename V>
void map_put(CompactMap<V>* map, uint64_t key, V val) {
	// Note keys cant be 0
	assert(key != 0);

	// Regrow map, if the hashmap is half full
	if (2 * map->item_count >= map->size) {
		map_grow(map, 2 * map->size);
	}
	assert(IS_POW2(map->size));

	size_t hash_index = MurmurHash3Mixer(key);
	// Linear probe
	for (;;) {
		// Since our map size is a power of 2, we can mod it using this bit trick
		// same has hash = hash % map->size;
		hash_index &= (map->size - 1);

		if (map->map[hash_index].key == 0) {
			// this is an empty slot
			// hash the key to this slot
			map->map[hash_index].key = key;
			map->map[hash_index].value = val;
			map->item_count++;
			return;
		} else if (map->map[hash_index].key == key) {
			// Override existing value
			map->map[hash_index].value = val;
			return;
		}
		hash_index++;
	}

}

template <typename V>
void map_grow(CompactMap<V>* map, size_t new_size) {

	new_size = MAX(new_size, 16);
	assert(IS_POW2(new_size));
	CompactMap<V> new_map;

	new_map.map = (CompactMapItem<V>*)calloc(new_size, sizeof(CompactMapItem<V>));
	new_map.size = new_size;
	new_map.item_count = 0;

	// Copy and rehash all the keys from the old map
	for (int i = 0; i < new_map.size; i++) {
		if (map->map && map->map[i].key) {// only hash if the key is non zero
			map_put(&new_map, map->map[i].key, map->map[i].value);
		}
	}

	// Free the old map, we are going to swap the old map with the new map
	free((void*)map->map);
	*map = new_map;
}

//template <typename V>
//V& CompactMap<V>::operator[] (uint64_t index) {
//	return map_get(this, index);
//}

//template struct CompactMap<uint64_t>;


