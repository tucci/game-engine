#pragma once

#include "Asset/Asset.h" 

#include <stdio.h>
#include <string.h>
#include "engine_platform.h"

#include "Common/common_macros.h"
#include "debug_macros.h"

void init_asset_tracker(AssetTracker* tracker) {
	map_init(&tracker->track_map);
	map_grow(&tracker->track_map, 16);
	arena_init(&tracker->mem);


	FILE* file;
	errno_t err;
	const char* track_file = ASSET_TRACKER_FILE;


	// try reading first
	// if the file doesnt exist, then we create it
	err = fopen_s(&file, track_file, "rb");


	if (err == 0) {
		// File exists
		// construct the tracker data from the file
		void* buffer[256];
		
		fread(buffer, sizeof(tracker->last_asset_id), 1, file);
		tracker->last_asset_id = *cast(u64*)buffer;

		fread(buffer, sizeof(tracker->assets_tracked), 1, file);
		tracker->assets_tracked = *cast(u64*)buffer;


		for (int i = 0; i < tracker->assets_tracked; i++) {
			AssetTrackData track_data;

			// Read the id
			fread(buffer, sizeof(u64), 1, file);
			AssetID id;
			id.id = *cast(u64*)buffer;

			// Read the filename length
			fread(buffer, sizeof(u32), 1, file);
			track_data.filename_length = *cast(u32*)buffer;


			// Read the filename str, and copy it our own memory storage
			fread(buffer, track_data.filename_length, 1, file);
			track_data.filename = cast(char*) arena_alloc(&tracker->mem, track_data.filename_length);
			snprintf(track_data.filename, track_data.filename_length, "%s", buffer);
			map_put(&tracker->track_map, id.id, track_data);
			
		}

		err = fclose(file);
		if (err == 0) {
			debug_print("Closed %s\n", track_file);
		} else {
			assert_fail();
			debug_print("Cannot close to %s\n", track_file);
		}

	} else {
		// File doesnt exist, don't do anything, the call to write_tracker_file, will create the file
	}

	

}

void destroy_asset_tracker(AssetTracker* tracker) {
	write_tracker_file(tracker);
	map_destroy(&tracker->track_map);
	arena_free(&tracker->mem);
}


bool is_asset_tracked(AssetTracker* tracker, char* filename) {
	AssetID id = find_asset_by_name(tracker, filename);

	if (id.id == 0 && id.type == AssetType_None) {
		// Asset is not tracked
		return false;
	}
	// Asset is found and tracked
	return true;
}

// filename should not include the .easset extension
AssetID find_asset_by_name(AssetTracker* tracker, const char* filename) {
	
	const char* base_file = platform_file_basename(filename);
	
	size_t filename_len = strlen(base_file);
	size_t map_size = tracker->track_map.size;
	// Go over our track map, and look for filename
	for (int i = 0; i < map_size; i++) {
		CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
		// Check if this is a valid track data
		if (track_item.key != 0 && track_item.key != TOMBSTONE) {
			if (strncmp(base_file, track_item.value.filename, filename_len) == 0) {
				AssetID id;
				id.id = track_item.key;
				return id;
			}
			
		}
	}
	// Not found
	AssetID id;
	id.id = 0;
	id.type = AssetType_None;
	return id;
}

// Tracking doesnt know anything about the asset type, it just tracks it, and provides the asset id
AssetID track_asset(AssetTracker* tracker, char* filename, u32 filename_length) {


	AssetID tracked_id = find_asset_by_name(tracker, filename);

	// Asset is already tracked, just return the same id
	if (tracked_id.id != 0) {
		return tracked_id;
	}

	AssetID id;
	id.id = next_asset_id(tracker);
	MapResult<AssetTrackData> result = map_get(&tracker->track_map, id.id);

	
	AssetTrackData track_data;
	if (result.found) {
		// This asset is already being, tracked and will be overrided
		track_data = result.value;
	} else {
		// New asset that hasnt been tracked before	
		tracker->assets_tracked++;
	}

	track_data.filename_length = filename_length;
	// We need to copy the filename to our own memory
	track_data.filename = cast(char*) arena_alloc(&tracker->mem, filename_length);
	snprintf(track_data.filename, filename_length, "%s", filename);

	// override or tracker this asset with the given id
	map_put(&tracker->track_map, id.id, track_data);
	
	return id;
}


static u64 next_asset_id(AssetTracker* tracker) {
	return tracker->last_asset_id++;
}

void write_tracker_file(AssetTracker* tracker) {
	

	// We are going to write the tracker data to a temp file first
	// if the writing fails half way, we wont lose the old track data

	// If we successfully write the track data, we will delete the old track file
	// and rename the temp file to the new track file

	// This prevents our track file from getting corrupted

	FILE* file;
	errno_t err;
	const char* track_temp_file = ASSET_TRACKER_TMP_SWAP_FILE;

	err = fopen_s(&file, track_temp_file, "wb");

	
	if (err == 0) {

		fwrite(cast(const void*) &tracker->last_asset_id, sizeof(tracker->last_asset_id), 1, file);
		fwrite(cast(const void*) &tracker->assets_tracked, sizeof(tracker->assets_tracked), 1, file);

		size_t map_size = tracker->track_map.size;
		// Go over our track map, and write the tracked assets to file
		for (int i = 0; i < map_size; i++) {
			CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
			// Dont write the item, if the asset id/key is either 0, or a tombstone(deleted value)
			if (track_item.key != 0 && track_item.key != TOMBSTONE) {
				// Write the asset id
				fwrite(cast(const void*) &track_item.key, sizeof(track_item.key), 1, file);
				// Write the length of the filename
				fwrite(cast(const void*) &track_item.value.filename_length, sizeof(track_item.value.filename_length), 1, file);
				// Write the actual filename string
				fwrite(cast(const void*) track_item.value.filename, track_item.value.filename_length, 1, file);
			}
		}

		
		

	} else {
		assert_fail();
	}

	err = fclose(file);
	if (err == 0) {
		debug_print("Finished writing to %s\n", track_temp_file);
		// Delete the old file and rename swap file to the original asset track file
		if (platform_pathfile_exists(ASSET_TRACKER_FILE)) {
			platform_file_delete(ASSET_TRACKER_FILE);
		} 
		platform_file_rename(ASSET_TRACKER_TMP_SWAP_FILE, ASSET_TRACKER_FILE);
		
		
	} else {
		assert_fail();
		debug_print("Cannot close to %s\n", track_temp_file);
	}
}


void init_scene_node(AssetImport_SceneNode* node, u64 id, char* name, u32 name_length) {
	node->id = id;
	node->name = name;
	node->name_length = name_length;
	//node->parent = NULL;
	node->children_count = 0;
	node->first_child = NULL;
	node->next_sibling = NULL;
	node->mesh_count = 0;
	node->meshes = NULL;
	node->texture_count = 0;
	node->textures = NULL;
}

void set_scene_node_name(AssetImport_SceneNode* node, char* name, u32 name_length) {
	node->name = name;
	node->name_length = name_length;
}

void add_child_to_scene_node(AssetImport_SceneNode* node, AssetImport_SceneNode* child) {
	// Attach the parent to the child node
	//child->parent = node;
	if (node->children_count == 0) {
		node->first_child = child;
	} else {
		// Find the end of the child list
		AssetImport_SceneNode* ptr = node->first_child;
		while (ptr->next_sibling != NULL) {
			ptr = ptr->next_sibling;
		}
		ptr->next_sibling = child;	
	}

	node->children_count++;
}

void set_scene_node_transform(AssetImport_SceneNode* node, Vec3f pos, Vec3f scale, Vec3f rot) {

	node->translation = pos;
	node->scale = scale;
	node->rotation = rot;

}

