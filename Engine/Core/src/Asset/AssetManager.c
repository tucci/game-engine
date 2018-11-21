#pragma once

#include "Asset/AssetManager.h"
#include "zlib.h"
#include "Common/common_macros.h"
#include "Common/stretchy_buffer.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>




void import_pak_file(AssetManager* manager, char* pak_file) {

}

void init_asset_manager(AssetManager* manager) {
	arena_init(&manager->asset_mem);
	map_init(&manager->asset_id_map);
	map_grow(&manager->asset_id_map, 16);

	init_asset_tracker(&manager->asset_tracker);
	manager->assets = NULL;

	manager->_scene_count = 0;
	manager->_static_mesh_count = 0;
	manager->_material_count = 0;
	manager->_light_count = 0;
	manager->_camera_count = 0;
	manager->_anim_count = 0;
	manager->_texture_count = 0;

	manager->_scenes = NULL;
	manager->_static_meshes = NULL;
	
}

void destroy_asset_manager(AssetManager* manager) {
	arena_free(&manager->asset_mem);
	map_destroy(&manager->asset_id_map);
	destroy_asset_tracker(&manager->asset_tracker);
	sb_free(manager->assets);


	// Clear all the internal data if needed, if it is not allocated with the arena
	sb_free(manager->_static_meshes);
}


InternalAsset get_asset_by_id(AssetManager* manager, AssetID id) {

	
	MapResult<u64> result = map_get(&manager->asset_id_map, id.id);
	u64 index_to_asset_type_array;
	if (!result.found) {
		// If it is not found, then it means this asset is not currently loaded into memory
		// Load it into memory
		MapResult<AssetTrackData> track_result = map_get(&manager->asset_tracker.track_map, id.id);
		if (!track_result.found) {
			// This id is currently not being tracked and is probably an error
			assert_fail();
		} else {
			import_asset_by_name(manager, track_result.value.filename);
			result = map_get(&manager->asset_id_map, id.id);
			index_to_asset_type_array = result.value;
		}
		
	} else {
		index_to_asset_type_array = result.value;
	}

	

	InternalAsset asset;
	asset.id = id;
	switch (id.type) {

		case AssetType_Scene:
			asset.scene = manager->_scenes[index_to_asset_type_array];
			break;
		case AssetType_StaticMesh:
			asset.mesh = manager->_static_meshes[index_to_asset_type_array];
			break;
	}

	return asset;
}

StaticMesh* get_static_mesh_by_id(AssetManager* manager, StaticMeshID id) {
	AssetID asset_id;
	asset_id.type = AssetType_StaticMesh;
	asset_id.mesh = id;
	InternalAsset mesh = get_asset_by_id(manager, asset_id);
	return mesh.mesh;
}

static AssetImport_SceneNode parse_scene_node(AssetManager* manager, FILE* file, void* buffer) {
	fread(buffer, sizeof(u32), 1, file);
	u32 node_size = *cast(u32*)buffer;
	


	//AssetSceneNode* node = cast(AssetSceneNode*) arena_alloc(&manager->asset_mem, sizeof(AssetSceneNode));
	AssetImport_SceneNode node;



	fread(buffer, sizeof(u64), 1, file);
	node.id = *cast(u64*)buffer;

	fread(buffer, sizeof(u32), 1, file);
	node.name_length = *cast(u32*)buffer;

	fread(buffer, node.name_length, 1, file);
	node.name = cast(char*) arena_alloc(&manager->asset_mem, node.name_length + 1);
	strcpy_s(node.name, node.name_length + 1, cast(char*)buffer);

	

	fread(buffer, sizeof(Vec3f), 1, file);
	node.translation = *cast(Vec3f*)buffer;

	fread(buffer, sizeof(Vec3f), 1, file);
	node.scale = *cast(Vec3f*)buffer;

	fread(buffer, sizeof(Vec3f), 1, file);
	node.rotation = *cast(Vec3f*)buffer;

	fread(buffer, sizeof(u32), 1, file);
	node.children_count = *cast(u32*)buffer;

	if (node.children_count > 0) {
		node.children = cast(AssetImport_SceneNode*) arena_alloc(&manager->asset_mem, node.children_count * sizeof(AssetImport_SceneNode));
		for (int i = 0; i < node.children_count; i++) {
			fread(buffer, sizeof(u64), 1, file);
			u64 child_node_id = *cast(u64*)buffer;
			node.children[i].id = child_node_id;
		}
	}

	fread(buffer, sizeof(u32), 1, file);
	node.mesh_count = *cast(u32*)buffer;
	if (node.mesh_count > 0) {
		node.meshes = cast(u32*) arena_alloc(&manager->asset_mem, node.mesh_count * sizeof(u32));
		for (int i = 0; i < node.mesh_count; i++) {
			fread(buffer, sizeof(u32), 1, file);
			node.meshes[i] = *cast(u32*)buffer;;
		}
	}

	if (node.children_count > 0) {
		for (int i = 0; i < node.children_count; i++) {
			node.children[i] = parse_scene_node(manager, file, buffer);
		}
	}
	

	return node;
}




AssetID import_asset_by_name(AssetManager* manager, char* filename) {
	AssetID asset;
	asset.id = 1;
	asset.type = AssetType_None;
	
	FILE* file;
	errno_t err;

	// TODO: use memory mapped files
	err = fopen_s(&file, filename, "rb");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		debug_print("Opening asset file %s,", filename);
	} else {
		debug_print("Cannot open asset file %s\n", filename);
		return asset;
	}

	void* buffer[256];


	// first 8 bytes are the asset id
	fread(buffer, 8, 1, file);
	asset.id = *(u64*)buffer;

	// next 4 bytes are the asset type
	fread(buffer, 4, 1, file);
	asset.type = *(AssetType*)buffer;
	

	switch (asset.type) {

		case AssetType_Scene: {
			

			AssetImport_Scene* scene = cast(AssetImport_Scene*)arena_alloc(&manager->asset_mem, sizeof(AssetImport_Scene));
			
			// Add this scene to the scene assets
			sb_push(manager->_scenes, scene);
			
			
			// Add this asset to the id map
			// map the asset id to the index in the scene array
			map_put(&manager->asset_id_map, asset.id, manager->_scene_count);

			manager->_scene_count++;
			
			

			fread(buffer, sizeof(u32), 1, file);
			scene->node_count = *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->mesh_count = *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->material_count = *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->light_count = *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->camera_count = *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->anim_count= *cast(u32*)buffer;

			fread(buffer, sizeof(u32), 1, file);
			scene->texture_count = *cast(u32*)buffer;

			if (scene->mesh_count > 0) {
				scene->mesh_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->mesh_count);
				for (int i = 0; i < scene->mesh_count; i++) {
					
					scene->mesh_infos[i].type = AssetType_StaticMesh;


					fread(buffer, sizeof(u64), 1, file);
					scene->mesh_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->mesh_infos[i]);
				}
			}

			if (scene->material_count > 0) {
				scene->material_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->material_count);
				for (int i = 0; i < scene->material_count; i++) {
					
					scene->material_infos[i].type = AssetType_Material;

					fread(buffer, sizeof(u64), 1, file);
					scene->material_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->material_infos[i]);
				}
			}

			if (scene->light_count > 0) {
				scene->light_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->light_count);
				for (int i = 0; i < scene->light_count; i++) {
					
					scene->light_infos[i].type = AssetType_Light;

					fread(buffer, sizeof(u64), 1, file);
					scene->light_infos[i].id = *cast(u64*)buffer;

					
					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->light_infos[i]);
				}
			}

			if (scene->camera_count > 0) {
				scene->camera_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->camera_count);
				for (int i = 0; i < scene->camera_count; i++) {
					
					scene->camera_infos[i].type = AssetType_Camera;
					
					fread(buffer, sizeof(u64), 1, file);
					scene->camera_infos[i].id = *cast(u64*)buffer;
					
					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->camera_infos[i]);
				}
			}

			if (scene->anim_count > 0) {
				scene->animation_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->anim_count);
				for (int i = 0; i < scene->camera_count; i++) {
					
					scene->animation_infos[i].type = AssetType_Animation;


					fread(buffer, sizeof(u64), 1, file);
					scene->animation_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->animation_infos[i]);
				}
			}

			if (scene->texture_count > 0) {
				scene->texture_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->texture_count);
				for (int i = 0; i < scene->camera_count; i++) {
					
					scene->texture_infos[i].type = AssetType_Texture;

					fread(buffer, sizeof(u64), 1, file);
					scene->texture_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					import_asset_by_id(manager, scene->texture_infos[i]);
				}
			}
			scene->root = cast(AssetImport_SceneNode*)arena_alloc(&manager->asset_mem, sizeof(AssetImport_SceneNode));
			*scene->root = parse_scene_node(manager, file, buffer);
			
			
			break;
		}

		case AssetType_StaticMesh: {

			
			
			
			
			StaticMesh* mesh = cast(StaticMesh*)arena_alloc(&manager->asset_mem, sizeof(StaticMesh));
			sb_push(manager->_static_meshes, mesh);

			// Add this asset to the id map
			// map the asset id to the index in the static mesh array
			map_put(&manager->asset_id_map, asset.id, manager->_static_mesh_count);
			manager->_static_mesh_count++;
			

			//asset.import_mesh = cast(AssetImport_StaticMesh*)arena_alloc(&manager->asset_mem, sizeof(AssetImport_StaticMesh));


			
			fread(buffer, sizeof(Vec3f), 1, file);
			//asset.import_mesh->translation = *cast(Vec3f*)buffer;
			fread(buffer, sizeof(Vec3f), 1, file);
			//asset.import_mesh->scale = *cast(Vec3f*)buffer;
			fread(buffer, sizeof(Vec3f), 1, file);
			//asset.import_mesh->rotation = *cast(Vec3f*)buffer;

			

			
			fread(buffer, 4, 1, file);
			//asset.import_mesh->mesh.vertex_count = *cast(int*)buffer;
			mesh->vertex_count = *cast(int*)buffer;

			fread(buffer, 4, 1, file);
			//asset.import_mesh->mesh.index_count = *cast(int*)buffer;
			mesh->index_count = *cast(int*)buffer;


			//size_t vertex_size = asset.import_mesh->mesh.vertex_count * sizeof(Vec3f);
			//size_t index_size = asset.import_mesh->mesh.index_count * sizeof(Vec3i);

			size_t vertex_size = mesh->vertex_count * sizeof(Vec3f);
			size_t index_size = mesh->index_count * sizeof(Vec3i);

			
			

			//asset.import_mesh->mesh.pos = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			//asset.import_mesh->mesh.indices = cast(Vec3i*) arena_alloc(&manager->asset_mem, index_size);
			//asset.import_mesh->mesh.normal = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			//asset.import_mesh->mesh.texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);
			////asset.import_mesh->mesh.lightmap_texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);

			mesh->pos = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			mesh->indices = cast(Vec3i*) arena_alloc(&manager->asset_mem, index_size);
			mesh->normal = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			mesh->texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);
			//mesh->lightmap_texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);

			//fread(asset.import_mesh->mesh.pos, vertex_size, 1, file);
			//fread(asset.import_mesh->mesh.indices, index_size, 1, file);
			//fread(asset.import_mesh->mesh.normal, vertex_size, 1, file);
			//fread(asset.import_mesh->mesh.texcoords, vertex_size, 1, file);
			////fread(asset.mesh->lightmap_texcoords, vertex_size, 1, file);

			fread(mesh->pos, vertex_size, 1, file);
			fread(mesh->indices, index_size, 1, file);
			fread(mesh->normal, vertex_size, 1, file);
			fread(mesh->texcoords, vertex_size, 1, file);
			//fread(mesh->lightmap_texcoords, vertex_size, 1, file);
			
			break;
		}
	}

	if (file) {

		err = fclose(file);
		if (err == 0) {
			debug_print("Closed asset file %s\n", filename);
		} else {
			debug_print("Cannot close asset file %s\n", filename);
		}
	}

	
	sb_push(manager->assets, asset);
	return asset;
}


void import_asset_by_id(AssetManager* manager, AssetID id) {
	MapResult<AssetTrackData> result = map_get(&manager->asset_tracker.track_map, id.id);
	if (!result.found) { assert_fail(); }
	import_asset_by_name(manager, result.value.filename);
}