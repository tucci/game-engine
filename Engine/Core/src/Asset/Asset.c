#pragma once

#include "Asset/Asset.h"

void init_scene_node(AssetSceneNode* node, char* name, uint32_t name_length) {
	node->name = name;
	node->name_length = name_length;
	//node->parent = NULL;
	node->first_child = NULL;
	node->next_sibling = NULL;
	node->meshes = NULL;
	node->children_count = 0;
	node->mesh_count = 0;
}

void set_scene_node_name(AssetSceneNode* node, char* name, uint32_t name_length) {
	node->name = name;
	node->name_length = name_length;
}

void add_child_to_scene_node(AssetSceneNode* node, AssetSceneNode* child) {
	// Attach the parent to the child node
	//child->parent = node;
	if (node->children_count == 0) {
		node->first_child = child;
	} else {
		// Find the end of the child list
		AssetSceneNode* ptr = node->first_child;
		while (ptr->next_sibling != NULL) {
			ptr = ptr->next_sibling;
		}
		ptr->next_sibling = child;	
	}

	node->children_count++;
}

void set_scene_node_transform(AssetSceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot) {
	Transform transform;
	transform.position = pos;
	transform.scale = scale;
	transform.rotation = euler_to_quat(rot);
	node->transform = trs_mat_from_transform(&transform);

}

void set_scene_node_transform(AssetSceneNode* node, Mat4x4f transform) {
	node->transform = transform;
}