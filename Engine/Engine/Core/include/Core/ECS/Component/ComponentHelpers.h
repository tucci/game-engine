#pragma once

#include "Core/ECS/Entity.h"
#include "types.h"
#include "Common/Map.h"
#include "Common/stretchy_buffer.h"





template<typename T>
bool comphelper_add_component_data(u64* total_count, u64* enabled_count, Entity entity, T** list, T con, u64 index) {
	u64 count = stb_sb_count((*list));
	if (*enabled_count == *total_count && count != *enabled_count) {
		(*list)[*enabled_count] = con;
	} else {
		stb_sb_push((*list), con);
	}

	T this_comp = (*list)[index];
	T first_disabled_comp = (*list)[*enabled_count];

	// swap the first disabled comp with the comp we are trying to enable
	(*list)[*enabled_count] = this_comp;
	(*list)[index] = first_disabled_comp;



	return true;

}

template<typename T>
bool comphelper_remove_component_data(Entity entity, T* list, u64 index, u64 swap_index) {

	// Get the last in the list to swap with
	T this_comp = list[index];
	T last_comp = list[swap_index];
	// swap the last at the current index we are removing from
	list[index] = last_comp;
	return true;
}


template<typename T>
void comphelper_enable_component(CompactMap<u64>* id_map, Entity* entitys, u64* enabled_count, T* list, Entity entity, bool enabled) {
	MapResult<u64> result = map_get(id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	u64 index = result.value;

	if (enabled) {


		T this_comp = list[index];
		T first_disabled_comp = list[(*enabled_count)];


		// swap the first disabled comp with the comp we are trying to enable
		list[(*enabled_count)] = this_comp;
		list[index] = first_disabled_comp;

		// Swap entitys
		Entity this_entity = entitys[index];
		Entity first_disabled_entity = entitys[(*enabled_count)];
		entitys[(*enabled_count)] = this_entity;
		entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(id_map, this_entity.id, (*enabled_count));
		map_put(id_map, first_disabled_entity.id, index);

		(*enabled_count)++;
	} else {


		T this_comp = list[index];
		T last_comp = list[(*enabled_count) - 1];
		// swap the last at the current index we are removing from
		list[index] = last_comp;
		list[(*enabled_count) - 1] = this_comp;

		// Swap entitys
		Entity this_entity = entitys[index];
		Entity first_disabled_entity = entitys[(*enabled_count) - 1];
		entitys[(*enabled_count) - 1] = this_entity;
		entitys[index] = first_disabled_entity;

		// Update the entity id to index mapping
		map_put(id_map, this_entity.id, (*enabled_count) - 1);
		map_put(id_map, first_disabled_entity.id, index);

		(*enabled_count)--;
	}
	

}

inline bool comphelper_is_component_enabled(CompactMap<u64>* id_map, u64* enabled_count, Entity entity) {
	MapResult<u64> result = map_get(id_map, entity.id);

	if (!result.found) {
		return false;
	}
	return result.value < (*enabled_count);
}