#pragma once

#include "Asset/Asset.h"

#include "Core/ECS/Component/StaticMesh.h"
#include "Common/Arena.h"



typedef struct AssetManager {
	Arena asset_mem;
	Asset* assets;
	CompactMap<uint64_t> map;
} AssetManager;



void init_asset_manager(AssetManager* manager);
void destroy_asset_manager(AssetManager* manager);

Asset get_asset_by_id(AssetManager* manager, uint64_t id);

Asset import_asset(AssetManager* manager, char* filename);
