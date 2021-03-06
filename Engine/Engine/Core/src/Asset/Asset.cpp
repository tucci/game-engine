#pragma once

#include "Asset/Asset.h" 

#include <stdio.h>
#include <string.h>


#include "Common/common_macros.h"
#include "debug_macros.h"
#include "Logger.h"

void init_asset_tracker(AssetTracker* tracker) {
	map_init(&tracker->track_map);
	map_grow(&tracker->track_map, 16);
	arena_init(&tracker->mem);

	read_tracker_file(tracker);
	

}

void destroy_asset_tracker(AssetTracker* tracker) {
	write_tracker_file(tracker);
	map_destroy(&tracker->track_map);
	arena_free(&tracker->mem);
}


bool is_asset_tracked(AssetTracker* tracker, char* filename) {
	AssetID id = find_asset_by_name(tracker, filename);

	if (id.id == 0 && id.type == AssetType::None) {
		// Asset is not tracked
		return false;
	}
	// Asset is found and tracked
	return true;
}
bool is_asset_tracked(AssetTracker* tracker, AssetID id) {
	MapResult<AssetTrackData> result = map_get(&tracker->track_map, id.id);
	return result.found;
}

void remove_all_tracked_assets(AssetTracker* tracker) {
	size_t map_size = tracker->track_map.size;
	// Go over our track map, and look for filename
	for (size_t i = 0; i < map_size; i++) {
		CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
		// Check if this is a valid track data
		if (track_item.key != 0 && track_item.key != TOMBSTONE) {
			platform_file_delete(track_item.value.file.buffer);
			map_remove(&tracker->track_map, track_item.key);
		}
	}
	tracker->last_asset_id = 1;
	tracker->assets_tracked = 0;
	write_tracker_file(tracker);
	
}

String name_of_asset(AssetTracker* tracker, AssetID id) {
	MapResult<AssetTrackData> result = map_get(&tracker->track_map, id.id);
	if (result.found) {
		return result.value.file;
	}
	String no_string("");
}

// filename should not include the .easset extension
AssetID find_asset_by_name(AssetTracker* tracker, String file) {
	
	char buffer[260];

	convert_to_os_path(file.buffer, buffer, 260);
	
	
	size_t filename_len = strlen(buffer);
	size_t map_size = tracker->track_map.size;
	// Go over our track map, and look for filename
	for (size_t i = 0; i < map_size; i++) {
		CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
		// Check if this is a valid track data
		if (track_item.key != 0 && track_item.key != TOMBSTONE) {
			// NOTE:
			// a filename could be something like "example_file"
			// but in the tracker the file is stored with the asset suffix
			// so if example file was a mesh it would be stored as "example_file_mesh.easset"
			// if we were to compare the given file to the tracker file it would never be equal
			// "example_file" != "example_file_mesh.easset"
			// so instead instead we do a subtring compare and ignore the suffix
			// while this is very nasty, it works for now
			// Once assets are put into pak files, and all string files are removed,
			// we will never need to compare file strings anymore so we wont have to worry about it
			// Usually in editor/debug mode we refer to assets by name
			// while in production/game mode, we refer to assets by ids/offsets into the pak files
			if (strncmp(buffer, track_item.value.file.buffer, filename_len) == 0) {
				AssetID id;
				id = track_item.value.assetid;
				return id;
			}
			
		}
	}
	// Not found
	AssetID id;
	id.id = 0;
	id.type = AssetType::None;
	
	return id;

}

AssetID track_asset(AssetTracker* tracker, AssetType asset_type, String file) {


	AssetID tracked_id = find_asset_by_name(tracker, file);

	// Asset is already tracked, just return the same id
	if (tracked_id.id != 0) {
		MapResult<AssetTrackData> result = map_get(&tracker->track_map, tracked_id.id);
		if (result.found) {
			AssetTrackData track_data;
			track_data = result.value;
			track_data.assetid.type = asset_type;
			// override or tracker this asset with the given id
			map_put(&tracker->track_map, tracked_id.id, track_data);
		}

		return tracked_id;
	}

	AssetID id;
	id.id = next_asset_id(tracker);
	id.type = asset_type;
	MapResult<AssetTrackData> result = map_get(&tracker->track_map, id.id);

	
	AssetTrackData track_data;
	if (result.found) {
		// This asset is already being tracked and will be overrided
		track_data = result.value;
	} else {
		// New asset that hasnt been tracked before	
		tracker->assets_tracked++;
	}

	track_data.assetid = id;

	// We need to copy the filename to our own memory
	char* filename_copy = cast(char*) arena_alloc(&tracker->mem, file.length);
	memcpy(filename_copy, file.buffer, file.length);

	track_data.file = String(filename_copy, file.length);
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

		u64 tracker_version = TRACKER_FILE_VERSION;
		fwrite(cast(const void*) &tracker_version, sizeof(tracker_version), 1, file);

		fwrite(cast(const void*) &tracker->last_asset_id, sizeof(tracker->last_asset_id), 1, file);
		fwrite(cast(const void*) &tracker->assets_tracked, sizeof(tracker->assets_tracked), 1, file);

		size_t map_size = tracker->track_map.size;
		// Go over our track map, and write the tracked assets to file
		for (size_t i = 0; i < map_size; i++) {
			CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
			// Dont write the item, if the asset id/key is either 0, or a tombstone(deleted value)
			if (track_item.key != 0 && track_item.key != TOMBSTONE) {
				// Write the asset id
				fwrite(cast(const void*) &track_item.key, sizeof(track_item.key), 1, file);

				// Write the asset type
				fwrite(cast(const void*) &track_item.value.assetid.type, sizeof(track_item.value.assetid.type), 1, file);

				char buffer[512];
				convert_to_os_path(track_item.value.file.buffer, buffer, track_item.value.file.length);

				// Write the length of the filename
				fwrite(cast(const void*) &track_item.value.file.length, sizeof(track_item.value.file.length), 1, file);
				// Write the actual filename string
				fwrite(cast(const void*) buffer, track_item.value.file.length, 1, file);
			}
		}

		
		

	} else {
		assert_fail();
	}

	err = fclose(file);
	if (err == 0) {
		LOG_INFO("ASSET MANAGER", "Finished writing to %s\n", track_temp_file);
		// Delete the old file and rename swap file to the original asset track file
		if (platform_pathfile_exists(ASSET_TRACKER_FILE)) {
			platform_file_delete(ASSET_TRACKER_FILE);
		} 
		platform_file_rename(ASSET_TRACKER_TMP_SWAP_FILE, ASSET_TRACKER_FILE);
		
		
	} else {
		LOG_FATAL("ASSET MANAGER", "Cannot close to %s\n", track_temp_file);
		assert_fail();
	}
}

static void read_tracker_file(AssetTracker* tracker) {
	FILE* file = NULL;
	errno_t err;
	const char* track_file = ASSET_TRACKER_FILE;


	// try reading first
	// if the file doesnt exist, then we create it
	err = fopen_s(&file, track_file, "rb");


	if (err == 0) {
		// File exists
		// construct the tracker data from the file
		void* buffer[256];

		u64 tracker_version = 0;
		fread(buffer, sizeof(tracker_version), 1, file);
		tracker_version = *cast(u64*)buffer;


		fread(buffer, sizeof(tracker->last_asset_id), 1, file);
		tracker->last_asset_id = *cast(u64*)buffer;

		fread(buffer, sizeof(tracker->assets_tracked), 1, file);
		tracker->assets_tracked = *cast(u64*)buffer;


		for (u64 i = 0; i < tracker->assets_tracked; i++) {
			AssetTrackData track_data;
			memset(&track_data, 0, sizeof(AssetTrackData));


			// Read the id
			fread(buffer, sizeof(u64), 1, file);
			track_data.assetid.id = *cast(u64*)buffer;

			fread(buffer, sizeof(AssetType), 1, file);
			track_data.assetid.type = *cast(AssetType*)buffer;
			

			
			
			// Read the filename length
			// Note that the lenth of String length is size_t
			fread(buffer, sizeof(size_t), 1, file);
			//track_data.filename_length = *cast(u32*)buffer;
			size_t filename_length = *cast(size_t*)buffer;


			// Read the filename str, and copy it our own memory storage
			fread(buffer, filename_length, 1, file);
			char* filename = cast(char*) arena_alloc(&tracker->mem, filename_length);
			//track_data.filename = cast(char*) arena_alloc(&tracker->mem, track_data.filename_length);
			memcpy(filename, buffer, filename_length);
			track_data.file = String(filename, filename_length);
			map_put(&tracker->track_map, track_data.assetid.id, track_data);

		}

		err = fclose(file);
		if (err == 0) {
			LOG_INFO("ASSET MANAGER", "Closed %s\n", track_file);
		}
		else {
			assert_fail();
			LOG_FATAL("ASSET MANAGER", "Cannot close to %s\n", track_file);
		}

	}
	else {
		// File doesnt exist, don't do anything, the call to write_tracker_file, will create the file
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
	node->material_count = 0;
	node->materials = NULL;
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
	node->get_rotation = rot;
}

