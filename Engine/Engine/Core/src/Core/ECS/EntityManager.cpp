#pragma once

#include "Core/ECS/EntityManager.h"
#include "Common/common_macros.h"



#include "Common/stretchy_buffer.h"

void init_component_manager(ComponentManager* cpm) {
	cpm->count = 0;
	cpm->types = NULL;
	map_init(&cpm->id_map);
}

void destroy_component_manager(ComponentManager* cpm) {
	cpm->count = 0;
	map_destroy(&cpm->id_map);
}

static void create_component_flag(ComponentManager* cpm, Entity entity) {
	
	ComponentType comp_type;
	comp_type = ComponentType::None;
	u64 count = stb_sb_count(cpm->types);
	if (count == cpm->count) {
		stb_sb_push(cpm->types, comp_type);
	} else {
		cpm->types[cpm->count] = comp_type;
	}
	map_put(&cpm->id_map, entity.id, cpm->count);
	cpm->count++;
}

static void remove_component_flag(ComponentManager* cpm, Entity entity) {
	// Get index of the entity we are trying to remove
	MapResult<u64> result = map_get(&cpm->id_map, entity.id);
	if (!result.found) {
		// This entity has already been deleted 
		return;
	}
	u64 index = result.value;

	ComponentType last_type = cpm->types[cpm->count - 1];
	// Place the last entity in the entity list at the index we are removing from
	cpm->types[index] = last_type;
	cpm->count--;
	// Update the mapping of the swapped entity to it's new index
	//map_put(&cpm->id_map, last_entity_in_list_to_swap_with.id, index);
	map_remove(&cpm->id_map, entity.id);

}

void set_component_flag(ComponentManager* cpm, Entity entity, ComponentType component) {
	MapResult<u64> result = map_get(&cpm->id_map, entity.id);
	if (!result.found) {
		assert_fail();
	}
	u64 index = result.value;
	ComponentType* flags = &cpm->types[index];
	*flags = *flags | component;
	
	
}

static void unset_component_flag(ComponentManager* cpm, Entity entity, ComponentType component) {
	MapResult<u64> result = map_get(&cpm->id_map, entity.id);
	if (!result.found) {
		assert_fail();
	}
	u64 index = result.value;
	ComponentType* flags = &cpm->types[index];
	*flags = *flags & ~component;
}



Entity create_entity(EntityManager* manager, String name) {
	
	
	int entity_index = manager->entitys_created;
	manager->entitys_created++;
	Entity entity;
	// Note our entites shouldnt start at 0, because our map expects that no key maps to 0
	entity.id = entity_index + 1;
	if (stb_sb_count(manager->entity_list) == manager->entity_count) {
		stb_sb_push(manager->entity_list, entity);
	} else {
		manager->entity_list[manager->entity_count] = entity;
	}
	

	// adds a mapping between the entity's id, and its index
	// while it seems dumb that the key,values are obvious when adding
	// when we delete an entity, we may need to remap its index to something else
	// mapping <entity_id, index into the entity array>
	map_put(&manager->entity_index_map, entity.id, manager->entity_count);
	manager->entity_count++;

	create_component_flag(&manager->component_manager, entity);

	entity_add_transform_component(&manager->transform_manager, entity, name);
	set_component_flag(&manager->component_manager, entity, ComponentType::Transform);

	
	
	

	return entity;
	

	
}

void destroy_entity(EntityManager* manager, Entity entity) {
	
	
	// Entity list before deleting [1, 2, 3, 4, 5]
	//                                          ^
	//                                     entity_count
	// Delete entity 3
	// Entity list after swapping [1, 2, 5, 4, 3]
	// Entity list after deleting [1, 2, 5, 4][5]
	//                                      ^
	//                                entity_count

	// Notice how [5] is still in the list at the end, we could have swapped 3 and 5
	// but it is not needed because we know how many entites we have
	// Note we are not shrinking the entity list array. it just keeps expanding untill the game/engines exits
	// because we are keeping track of how many items we have in the list
	// we differentiate between the array size, and how many entities are in the array




	
	
	// Get index of the entity we are trying to remove
	MapResult<u64> result = map_get(&manager->entity_index_map, entity.id);
	if (!result.found) {
		// This entity has already been deleted 
		return;
	}
	u64 index = result.value;
	
	Entity last_entity_in_list_to_swap_with = manager->entity_list[manager->entity_count - 1];
	// Place the last entity in the entity list at the index we are removing from
	manager->entity_list[index] = last_entity_in_list_to_swap_with;
	manager->entity_count--;
	// Update the mapping of the swapped entity to it's new index
	map_put(&manager->entity_index_map, last_entity_in_list_to_swap_with.id, index);
	map_remove(&manager->entity_index_map, entity.id);
	

	// Need to remove it from all the other components


	for (int i = (int)ComponentType::None; i < (int)ComponentType::Count; i++) {
		ComponentType type = (ComponentType)i;
		remove_component(manager, entity, type);
	}

	remove_component_flag(&manager->component_manager, entity);

}

void add_component(EntityManager* manager, Entity entity, ComponentType type) {

	// Get index of the entity we are trying to remove
	MapResult<u64> result = map_get(&manager->entity_index_map, entity.id);
	if (!result.found) {
		// This entity has already been deleted 
		return;
	}

	set_component_flag(&manager->component_manager, entity, type);

	switch (type) {
		case ComponentType::Transform: {
			//entity_add_transform_component(&manager->transform_manager, entity);
			break;
		}
		case ComponentType::Camera: {
			entity_add_camera_component(&manager->camera_manager, entity);
			break;
		}
		case ComponentType::StaticMesh: {
			entity_add_mesh_component(&manager->static_mesh_manger, entity);
			break;
		}
		case ComponentType::Light: {
			entity_add_light_component(&manager->light_manager, entity);
			break;
		}

		case ComponentType::Render: {
			entity_add_render_component(&manager->render_manager, entity);
			break;
		}
	}
}


void remove_component(EntityManager* manager, Entity entity, ComponentType type) {
	unset_component_flag(&manager->component_manager, entity, type);

	switch (type) {
		case ComponentType::Transform: {
			entity_remove_transform_component(&manager->transform_manager, entity);
			break;
		}
		case ComponentType::Camera: {
			entity_remove_camera_component(&manager->camera_manager, entity);
			break;
		}
		case ComponentType::StaticMesh: {
			entity_remove_mesh_component(&manager->static_mesh_manger, entity);
			break;
		}
		case ComponentType::Light: {
			entity_remove_light_component(&manager->light_manager, entity);
			break;
		}
		case ComponentType::Render: {
			entity_remove_render_component(&manager->render_manager, entity);
			break;
		}
	}
}


bool has_component(EntityManager* manager, Entity entity, ComponentType component) {
	ComponentManager* cpm = &manager->component_manager;
	MapResult<u64> result = map_get(&cpm->id_map, entity.id);
	if (!result.found) {
		assert_fail();
	}
	u64 index = result.value;
	ComponentType flags = cpm->types[index];
	return ((u64)flags & (u64)component);
}


void init_entity_manager(EntityManager* manager) {
	arena_init(&manager->arena);

	init_component_manager(&manager->component_manager);

	manager->entity_count = 0;
	manager->entitys_created = 0;
	manager->entity_list = NULL;
	map_init(&manager->entity_index_map);


	size_t mem_size = ECS_MEMORY;
	void* mem_block = arena_alloc(&manager->arena, mem_size);
	mem_size = manager->arena.end - cast(char*) mem_block;
	stack_alloc_init(&manager->stack_mem, mem_block, mem_size);
	
	
	init_transform_manager(&manager->transform_manager);
	init_static_mesh_manager(&manager->static_mesh_manger);
	init_camera_manager(&manager->camera_manager);
	init_light_manager(&manager->light_manager);
	init_render_manager(&manager->render_manager);

	

}

void destroy_entity_manager(EntityManager* manager) {
	arena_free(&manager->arena);
	stb_sb_free(manager->entity_list);

	destroy_component_manager(&manager->component_manager);
	
	destroy_transform_manager(&manager->transform_manager);
	destroy_static_mesh_manager(&manager->static_mesh_manger);
	destroy_camera_manager(&manager->camera_manager);
	destroy_light_manager(&manager->light_manager);
	destroy_render_manager(&manager->render_manager);
}

void attach_child_entity(EntityManager* manager, Entity entity, Entity child) {

	if (entity.id == child.id) {
		// An entity can't be a child of itself
		return;
	}

	// This entity was not found. 
	// Just do nothing, and return back
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { return; }
	
	
	// Get the mapped index into the first child array
	
	u64 entity_index = result.value;

	Entity* first_child = &manager->transform_manager.first_child[entity_index];
	if (first_child->id == NO_ENTITY_ID) {
		// This entity has no children, so we need to add this child to the first child
		first_child->id = child.id;
	} else {
		// This entity already has a first child so we need to look at the siblings
		// Try to get the mapped index for the child entity
		MapResult<u64> child_result = map_get(&manager->transform_manager.id_map, first_child->id);
		// This index is not in the manager
		if (!child_result.found) { return; }
		u64 first_child_index = child_result.value;

		// Get the next sibling entity mapped to the child index by using the next_sibling array
		Entity* next_sibling = &manager->transform_manager.next_sibling[first_child_index];


		// Loop over all the siblings untill we get to the end of the list
		while (next_sibling->id != NO_ENTITY_ID) {
			// Get next sibling
			child_result = map_get(&manager->transform_manager.id_map, next_sibling->id);
			// if the entity is not valid do nothing
			if (!child_result.found) { return; }
			// move the next_sibling entity to the next one
			next_sibling = &manager->transform_manager.next_sibling[child_result.value];
		}
		next_sibling->id = child.id;
		
	}
	MapResult<u64> child_result = map_get(&manager->transform_manager.id_map, child.id);
	if (!child_result.found) {
		return;
	}

	// Assign the child, the parent entity
	u64 child_index = child_result.value;
	
	manager->transform_manager.parent[child_index] = entity;
	manager->transform_manager.child_count[entity_index]++;
}

u64 entity_count(EntityManager* manager) {
	u64 count = manager->entity_count;
	return count;
}

Entity parent(EntityManager* manager, Entity entity) {
	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) {
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	Entity parent_entity = manager->transform_manager.parent[index];
	return parent_entity;
}


Entity first_child(EntityManager* manager, Entity entity) {

	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { 
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	// Get the first child for this entity
	Entity first = manager->transform_manager.first_child[index];
	return first;
}

Entity next_sibling(EntityManager* manager, Entity entity) {
	// Get the mapped index for this entity
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) {
		Entity no_entity;
		no_entity.id = NO_ENTITY_ID;
		return no_entity;
	}
	u64 index = result.value;

	// Get the next sibling for this entity
	Entity next = manager->transform_manager.next_sibling[index];
	return next;
}

u64 children_count(EntityManager* manager, Entity entity) {
	// This entity was not found. 
	// Just do nothing, and return back
	MapResult<u64> result = map_get(&manager->transform_manager.id_map, entity.id);
	if (!result.found) { return 0; }
	// Get the mapped index into the first child array
	u64 entity_index = result.value;

	u64 count = manager->transform_manager.child_count[entity_index];
	return count;
}

u64 get_index_for_entity(EntityManager* manager, Entity entity, CompactMap<u64>* map) {
	MapResult<u64> result = map_get(map, entity.id);
	assert(result.found);
	if (!result.found) {}
	int index = result.value;
	return index;
}



//static void import_asset_scene_node(EntityManager* manager, AssetImport_Scene* scene, AssetImport_SceneNode* parent_node, Entity parent_entity) {
//
//	AssetImport_SceneNode* children = parent_node->children;
//	for (u32 i = 0; i < parent_node->children_count; i++) {
//		AssetImport_SceneNode* child_node = &children[i];
//		Entity child_entity = create_entity(manager, child_node->name);
//
//
//
//
//		//add_component(manager, child_entity, ComponentType::Transform);
//		//set_name(manager, child_entity, child_node->name);
//		set_position(manager, child_entity, child_node->translation);
//		set_scale(manager, child_entity, child_node->scale);
//		set_rotation(manager, child_entity, euler_to_quat(child_node->get_rotation));
//		attach_child_entity(manager, parent_entity, child_entity);
//
//		import_asset_scene_node(manager, scene, child_node, child_entity);
//	}
//
//	if (parent_node->mesh_count > 0) {
//		for (u32 i = 0; i < parent_node->mesh_count; i++) {
//			add_component(manager, parent_entity, ComponentType::StaticMesh);
//			add_component(manager, parent_entity, ComponentType::Render);
//			set_render_visibility(manager, parent_entity, true);
//
//
//			u32 mesh_index = parent_node->meshes[i];
//			AssetID mesh_id = scene->mesh_infos[mesh_index];
//			set_static_mesh(manager, parent_entity, mesh_id.mesh);
//
//			// TODO: What happens when the mesh has multiple child nodes, does that mean is has multiple materials?
//			//assert(parent_node->children_count < 2);
//			// Get material for this mesh
//			AssetImport_SceneNode* child_node = &parent_node->children[i];
//			if (child_node->material_count > 0) {
//				u32 mat_index = child_node->materials[0];
//				AssetID material_id = scene->material_infos[mat_index];
//				set_render_material(manager, parent_entity, material_id.material);
//			} else {
//
//				DEBUG_BREAK;
//				// use default material
//				//set_render_material(manager)
//
//			}
//
//		}
//	}
//
//}
//
//
//Entity import_asset_scene(EntityManager* manager, SceneID id) {
//
//	AssetID scene_id;
//	scene_id.id = id.id;
//	scene_id.type = AssetType::Scene;
//	InternalAsset asset = get_asset_by_id(game->engineAPI.asset_manager, scene_id);
//	
//	AssetImport_Scene* scene = asset.scene;
//	EntityManager* manager = game->engineAPI.entity_manager;
//
//	
//	Entity root = create_entity(manager, scene->root->name);
//	//add_component(manager, root, ComponentType::Transform);
//	//set_name(manager, root, scene->root->name);
//
//	set_position(manager, root, scene->root->translation);
//	set_scale(manager, root, scene->root->scale);
//	set_rotation(manager, root, euler_to_quat(scene->root->get_rotation));
//
//	
//	import_asset_scene_node(manager, scene, scene->root, root);
//	return root;
//}
//
//

