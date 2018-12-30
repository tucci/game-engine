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
	size_t stack_size = MEGABYTES(1);
	void* start = arena_alloc(&manager->asset_mem, stack_size);
	stack_alloc_init(&manager->stack, start, stack_size);
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
	manager->_materials = NULL;
	manager->_textures = NULL;
	
}

void destroy_asset_manager(AssetManager* manager) {
	arena_free(&manager->asset_mem);
	stack_reset(&manager->stack);
	map_destroy(&manager->asset_id_map);
	destroy_asset_tracker(&manager->asset_tracker);
	sb_free(manager->assets);

	// Clear all the internal data if needed, if it is not allocated with the arena
	sb_free(manager->_scenes);
	sb_free(manager->_static_meshes);
	sb_free(manager->_materials);
	sb_free(manager->_textures);
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
			load_asset_by_name(manager, track_result.value.filename);
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
		case AssetType_Material:
			asset.material = manager->_materials[index_to_asset_type_array];
			break;
		case AssetType_Texture:
			asset.texture = manager->_textures[index_to_asset_type_array];
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

Material* get_material_by_id(AssetManager* manager, MaterialID id) {
	AssetID asset_id;
	asset_id.type = AssetType_Material;
	asset_id.material = id;
	InternalAsset mat = get_asset_by_id(manager, asset_id);
	return mat.material;
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

	node.material_count = 0;
	node.materials = NULL;
	node.texture_count = 0;
	node.textures = NULL;

	if (node.children_count > 0) {
		node.children = cast(AssetImport_SceneNode*) arena_alloc(&manager->asset_mem, node.children_count * sizeof(AssetImport_SceneNode));
		for (u32 i = 0; i < node.children_count; i++) {
			fread(buffer, sizeof(u64), 1, file);
			u64 child_node_id = *cast(u64*)buffer;
			node.children[i].id = child_node_id;
		}
	}

	// Read meshes
	fread(buffer, sizeof(u32), 1, file);
	node.mesh_count = *cast(u32*)buffer;
	if (node.mesh_count > 0) {
		node.meshes = cast(u32*) arena_alloc(&manager->asset_mem, node.mesh_count * sizeof(u32));
		for (u32 i = 0; i < node.mesh_count; i++) {
			fread(buffer, sizeof(u32), 1, file);
			node.meshes[i] = *cast(u32*)buffer;;
		}
	}

	// Read materials
	fread(buffer, sizeof(u32), 1, file);
	node.material_count = *cast(u32*)buffer;
	if (node.material_count > 0) {
		node.materials = cast(u32*) arena_alloc(&manager->asset_mem, node.material_count * sizeof(u32));
		for (u32 i = 0; i < node.material_count; i++) {
			fread(buffer, sizeof(u32), 1, file);
			node.materials[i] = *cast(u32*)buffer;;
		}
	}


	// Read textures
	fread(buffer, sizeof(u32), 1, file);
	node.texture_count = *cast(u32*)buffer;
	if (node.texture_count > 0) {
		node.textures= cast(u32*) arena_alloc(&manager->asset_mem, node.texture_count* sizeof(u32));
		for (u32 i = 0; i < node.texture_count; i++) {
			fread(buffer, sizeof(u32), 1, file);
			node.textures[i] = *cast(u32*)buffer;;
		}
		
	}

	// Recursivly parse the child nodes
	if (node.children_count > 0) {
		for (u32 i = 0; i < node.children_count; i++) {
			node.children[i] = parse_scene_node(manager, file, buffer);
		}
	}
	

	return node;
}




AssetID load_asset_by_name(AssetManager* manager, char* filename) {
	AssetID asset;
	asset.id = 0;
	asset.status = AssetStatus_Unloaded;
	asset.type = AssetType_None;
	
	FILE* file;
	errno_t err;

	
	char buf[260];
	IString ifilename(filename);
	platform_file_dirname(ifilename, buf, 260);
	IString path(buf);

	

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
			scene->id = asset.scene;
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
				for (u32 i = 0; i < scene->mesh_count; i++) {
					
					scene->mesh_infos[i].type = AssetType_StaticMesh;


					fread(buffer, sizeof(u64), 1, file);
					scene->mesh_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					load_asset_by_id(manager, scene->mesh_infos[i]);
				}
			}

			if (scene->material_count > 0) {
				scene->material_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->material_count);
				for (u32 i = 0; i < scene->material_count; i++) {
					
					scene->material_infos[i].type = AssetType_Material;

					fread(buffer, sizeof(u64), 1, file);
					scene->material_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					load_asset_by_id(manager, scene->material_infos[i]);
				}
			}

			if (scene->light_count > 0) {
				scene->light_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->light_count);
				for (u32 i = 0; i < scene->light_count; i++) {
					
					scene->light_infos[i].type = AssetType_Light;

					fread(buffer, sizeof(u64), 1, file);
					scene->light_infos[i].id = *cast(u64*)buffer;

					
					// TODO: push this to a import queue on a seperate thread
					load_asset_by_id(manager, scene->light_infos[i]);
				}
			}

			if (scene->camera_count > 0) {
				scene->camera_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->camera_count);
				for (u32 i = 0; i < scene->camera_count; i++) {
					
					scene->camera_infos[i].type = AssetType_Camera;
					
					fread(buffer, sizeof(u64), 1, file);
					scene->camera_infos[i].id = *cast(u64*)buffer;
					
					// TODO: push this to a import queue on a seperate thread
					load_asset_by_id(manager, scene->camera_infos[i]);
				}
			}

			if (scene->anim_count > 0) {
				scene->animation_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->anim_count);
				for (u32 i = 0; i < scene->anim_count; i++) {
					
					scene->animation_infos[i].type = AssetType_Animation;


					fread(buffer, sizeof(u64), 1, file);
					scene->animation_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					load_asset_by_id(manager, scene->animation_infos[i]);
				}
			}

			if (scene->texture_count > 0) {
				scene->texture_infos = cast(AssetID*) arena_alloc(&manager->asset_mem, sizeof(AssetID) * scene->texture_count);
				for (u32 i = 0; i < scene->texture_count; i++) {
					
					scene->texture_infos[i].type = AssetType_Texture;

					fread(buffer, sizeof(u64), 1, file);
					scene->texture_infos[i].id = *cast(u64*)buffer;

					// TODO: push this to a import queue on a seperate thread
					//load_asset_by_id(manager, scene->texture_infos[i]);
				}
			}
			scene->root = cast(AssetImport_SceneNode*)arena_alloc(&manager->asset_mem, sizeof(AssetImport_SceneNode));
			*scene->root = parse_scene_node(manager, file, buffer);
			
			
			break;
		}

		case AssetType_StaticMesh: {

			
			
			
			
			StaticMesh* mesh = cast(StaticMesh*)arena_alloc(&manager->asset_mem, sizeof(StaticMesh));
			mesh->id = asset.mesh;
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
		case AssetType_Material: {

			Material* material = cast(Material*)arena_alloc(&manager->asset_mem, sizeof(Material));
			init_material_defaults(material);
			material->id = asset.material;
			sb_push(manager->_materials, material);

			
			// Add this asset to the id map
			// map the asset id to the index in the material array
			map_put(&manager->asset_id_map, asset.id, manager->_material_count);
			manager->_material_count++;

			// Read name
			fread(buffer, sizeof(s32), 1, file);
			s32 name_length = *cast(s32*)buffer;
			fread(buffer, name_length, 1, file);
			char* name = cast(char*) arena_alloc(&manager->asset_mem, name_length);
			snprintf(name, name_length, "%s", buffer);

			// Read shading model
			fread(buffer, sizeof(material->shading_model), 1, file);
			material->shading_model = *cast(MaterialShadingModel*)buffer;

			
			// Read metallic factor
			fread(buffer, sizeof(material->metallic_factor), 1, file);
			material->metallic_factor = *cast(float*)buffer;

			// Read roughness factor
			fread(buffer, sizeof(material->roughness_factor), 1, file);
			material->roughness_factor = *cast(float*)buffer;

			// Read emissive factor
			fread(buffer, sizeof(material->emissive_factor), 1, file);
			material->emissive_factor = *cast(float*)buffer;
			
			
			// albedo color
			fread(buffer, sizeof(Vec3f), 1, file);
			material->albedo_color = *cast(Vec3f*)buffer;
			// emissive color
			fread(buffer, sizeof(Vec3f), 1, file);
			material->emissive_color = *cast(Vec3f*)buffer;

			
			
			
			

			// Read layered texture count
			fread(buffer, sizeof(s32), 1, file);
			s32 layered_texture_count = *cast(s32*)buffer;
			for (s32 i = 0; i < layered_texture_count; i++) {
				// texture type
				fread(buffer, sizeof(TextureType), 1, file);
				TextureType texture_type = *cast(TextureType*)buffer;
				// Read asset id
				fread(buffer, sizeof(AssetID), 1, file);
				AssetID texture_id = *cast(AssetID*)buffer;
				

				// TODO: push onto import queue
				load_asset_by_id(manager, texture_id);

				// Once the dependant texture is loaded, we need update the internal pointer to the texture inside the material
				InternalAsset asset = get_asset_by_id(manager, texture_id);
				update_material_with_texture_data(material, texture_type, texture_id.texture, asset.texture, 0);

				// NOTE: our material doesnt support layerted materials yet
				

				//// inner texture count
				//fread(buffer, sizeof(s32), 1, file);
				//s32 texture_count = *cast(s32*)buffer;
				//for (s32 j = 0; j < texture_count; j++) {
				//	// Read the texture asset ids
				//	fread(buffer, sizeof(AssetID), 1, file);
				//	AssetID texture_id = *cast(AssetID*)buffer;
				//	set_texture_type_and_id(material, texture_type, texture_id.texture, j);
				//	// TODO: push onto import queue
				//	import_asset_by_id(manager, texture_id);
				//}

			}

			
			
			


			break;
		}
		case AssetType_Texture: {
			Texture2D* texture = cast(Texture2D*)arena_alloc(&manager->asset_mem, sizeof(Texture2D));
			sb_push(manager->_textures, texture);
			
			// Add this asset to the id map
			// map the asset id to the index in the texture array
			map_put(&manager->asset_id_map, asset.id, manager->_texture_count);
			texture->id = asset.texture;
			manager->_texture_count++;
			
			
			
			// Read name
			fread(buffer, sizeof(s32), 1, file);
			s32 name_length = *cast(s32*)buffer;
			fread(buffer, name_length, 1, file);
			char* name = cast(char*) arena_alloc(&manager->asset_mem, name_length);
			snprintf(name, name_length + 1, "%s", buffer);
			
			
			
			
			// Read filename
			fread(buffer, sizeof(s32), 1, file);
			s32 filename_length = *cast(s32*)buffer;
			fread(buffer, filename_length, 1, file);
			char* texture_filename = cast(char*) arena_alloc(&manager->asset_mem, filename_length);
			snprintf(texture_filename, filename_length + 1, "%s", buffer);
			
			
			
			// Read relative filename
			fread(buffer, sizeof(s32), 1, file);
			s32 relative_filename_length = *cast(s32*)buffer;
			fread(buffer, relative_filename_length, 1, file);
			char* relative_filename = cast(char*) arena_alloc(&manager->asset_mem, relative_filename_length);
			snprintf(relative_filename, relative_filename_length + 1, "%s", buffer);






			fread(buffer, sizeof(Vec3f), 1, file);
			Vec3f translation = *cast(Vec3f*)buffer;
			
			fread(buffer, sizeof(Vec2f), 1, file);
			texture->uv_translation = *cast(Vec2f*)buffer;
			
			fread(buffer, sizeof(Vec2f), 1, file);
			texture->uv_scaling = *cast(Vec2f*)buffer;
			
			fread(buffer, sizeof(float), 1, file);
			texture->uv_rotation = *cast(float*)buffer;
			
			// Read texture width,height, channels
			fread(buffer, sizeof(texture->width), 1, file);
			texture->width = *cast(s32*)buffer;

			fread(buffer, sizeof(texture->height), 1, file);
			texture->height= *cast(s32*)buffer;

			fread(buffer, sizeof(texture->channels), 1, file);
			texture->channels = *cast(s32*)buffer;

			fread(buffer, sizeof(texture->depth), 1, file);
			texture->depth = *cast(s32*)buffer;

			// Calculate image size based on dimensions
			size_t image_size = texture->width * texture->height * texture->channels;

			if (image_size > 0) {
				// Allocate space for the texture
				texture->data = (unsigned char*)arena_alloc(&manager->asset_mem, image_size);
				// read texture image from file to the texture struct
				fread(texture->data, image_size, 1, file);
			} else {
				// TODO: handle empty textures
			}
			

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


void load_asset_by_id(AssetManager* manager, AssetID id) {
	MapResult<AssetTrackData> result = map_get(&manager->asset_tracker.track_map, id.id);
	if (!result.found) { assert_fail(); }
	
	load_asset_by_name(manager, result.value.filename);
}


MaterialID create_material(AssetManager* manager, IString path, IString name, Material* mat) {
	

	// Get the size of the full path
	// + 1 file seperator
	// + 4 _mat part
	// + 1 for null terminator
	u32 str_size = path.length + 1 + 4 + name.length + ASSET_FILE_EXTENSION_LENGTH + 1;


	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&manager->stack, str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, str_size, "%s_mat%s", name.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, str_size);


	// Track the material asset into our system
	AssetID id = track_asset(&manager->asset_tracker, file_str, str_size);
	id.type = AssetType_Material;

	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary
										   // Write the material asset to disk
	if (err == 0) {
		debug_print("Writing to %s,", file_str);
	} else {
		assert_fail();
		debug_print("Fail writing to %s,", file_str);
	}
	


	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	// Write type of asset
	AssetType type = AssetType_Material;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);



	
	// Write material name length
	fwrite(cast(const void*) &name.length, sizeof(name.length), 1, file);
	//// Write material name 
	fwrite(cast(const void*) name.buf, name.length, 1, file);


	// Write shading model type
	fwrite(cast(const void*) &mat->shading_model, sizeof(mat->shading_model), 1, file);

	// Write factors
	fwrite(cast(const void*) &mat->metallic_factor, sizeof(mat->metallic_factor), 1, file);
	fwrite(cast(const void*) &mat->roughness_factor, sizeof(mat->roughness_factor), 1, file);
	fwrite(cast(const void*) &mat->emissive_factor, sizeof(mat->emissive_factor), 1, file);


	// Write colors
	fwrite(cast(const void*) &mat->albedo_color, sizeof(mat->albedo_color), 1, file);
	fwrite(cast(const void*) &mat->emissive_color, sizeof(mat->emissive_color), 1, file);


	s32 texture_count = 5;
	// Write texture count
	fwrite(cast(const void*) &texture_count, sizeof(texture_count), 1, file);

	// Write the texture type and ids
	
	fwrite(cast(const void*) TextureType_Albedo, sizeof(TextureType_Albedo), 1, file);
	fwrite(cast(const void*) &mat->albedo->id, sizeof(mat->albedo->id), 1, file);
		
	fwrite(cast(const void*) TextureType_Normal, sizeof(TextureType_Normal), 1, file);
	fwrite(cast(const void*) &mat->normal->id, sizeof(mat->normal->id), 1, file);

	fwrite(cast(const void*) TextureType_Metal, sizeof(TextureType_Metal), 1, file);
	fwrite(cast(const void*) &mat->metal->id, sizeof(mat->metal->id), 1, file);

	fwrite(cast(const void*) TextureType_Roughness, sizeof(TextureType_Roughness), 1, file);
	fwrite(cast(const void*) &mat->roughness->id, sizeof(mat->roughness->id), 1, file);

	fwrite(cast(const void*) TextureType_AO, sizeof(TextureType_AO), 1, file);
	fwrite(cast(const void*) &mat->ao->id, sizeof(mat->ao->id), 1, file);



	err = fclose(file);
	if (err == 0) {
		debug_print("Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		debug_print("Cannot close to %s\n", file_str);
	}


	stack_pop(&manager->stack);


	return id.material;

}

//TextureID create_texture(AssetManager* manager, IString path, IString name, Texture2D* texture) {
//	// TODO: implement texture creating
//	
//}
