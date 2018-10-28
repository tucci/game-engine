#pragma once

#include "Asset/AssetManager.h"
#include "zlib.h"
#include "Common/common_macros.h"
#include "Common/stretchy_buffer.h"

void init_asset_manager(AssetManager* manager) {
	arena_init(&manager->asset_mem);
	map_init(&manager->map);
	manager->assets = NULL;
	
}

void destroy_asset_manager(AssetManager* manager) {
	arena_free(&manager->asset_mem);
	map_destroy(&manager->map);
	sb_free(manager->assets);
}

Asset get_asset_by_id(AssetManager* manager, uint64_t id) {
	MapResult<uint64_t> result = map_get(&manager->map, id);
	if (result.found) {
		uint64_t index = result.value;
		return manager->assets[index];
	} 
	Asset none;
	none.id = -1;
	none.type = AssetType_None;
	none.import_mesh = NULL;
	return none;
	
}

Asset import_asset(AssetManager* manager, char* filename) {
	Asset asset;
	asset.id = -1;
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


	fread(buffer, 4, 1, file);
	asset.type = *(AssetType*)buffer;
	switch (asset.type) {


		case AssetType_StaticMesh: {

			
			asset.import_mesh = cast(AssetImport_StaticMesh*)arena_alloc(&manager->asset_mem, sizeof(AssetImport_StaticMesh));


			fread(buffer, sizeof(Vec3f), 1, file);
			asset.import_mesh->translation = *cast(Vec3f*)buffer;
			fread(buffer, sizeof(Vec3f), 1, file);
			asset.import_mesh->rotation = *cast(Vec3f*)buffer;
			fread(buffer, sizeof(Vec3f), 1, file);
			asset.import_mesh->scale = *cast(Vec3f*)buffer;

			

			
			fread(buffer, 4, 1, file);
			
			asset.import_mesh->mesh.vertex_count = *cast(int*)buffer;

			fread(buffer, 4, 1, file);
			asset.import_mesh->mesh.index_count = *cast(int*)buffer;

			//fread(buffer, 4, 1, file);
			//asset.mesh->normal_count = *cast(int*)buffer;

			//fread(buffer, 4, 1, file);
			//asset.mesh->uv_count = *cast(int*)buffer;

			//fread(buffer, 4, 1, file);
			//asset.mesh->uv_index_count = *cast(int*)buffer;

			size_t vertex_size = asset.import_mesh->mesh.vertex_count * sizeof(Vec3f);
			size_t index_size = asset.import_mesh->mesh.index_count * sizeof(Vec3i);

			//size_t normal_size = asset.mesh->normal_count * sizeof(Vec3f);
			//size_t uv_size = asset.mesh->uv_count * sizeof(Vec2f);
			

			asset.import_mesh->mesh.pos = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			asset.import_mesh->mesh.indices = cast(Vec3i*) arena_alloc(&manager->asset_mem, index_size);
			asset.import_mesh->mesh.normal = cast(Vec3f*) arena_alloc(&manager->asset_mem, vertex_size);
			asset.import_mesh->mesh.texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);
			asset.import_mesh->mesh.lightmap_texcoords = cast(Vec2f*) arena_alloc(&manager->asset_mem, vertex_size);

			fread(asset.import_mesh->mesh.pos, vertex_size, 1, file);
			fread(asset.import_mesh->mesh.indices, index_size, 1, file);
			fread(asset.import_mesh->mesh.normal, vertex_size, 1, file);
			fread(asset.import_mesh->mesh.texcoords, vertex_size, 1, file);
			//fread(asset.mesh->lightmap_texcoords, vertex_size, 1, file);
			
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


