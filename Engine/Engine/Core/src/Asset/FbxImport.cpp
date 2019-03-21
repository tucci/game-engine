#pragma once

#include "Asset/FbxImport.h"

#include <stdio.h>
#include <string.h>
#include "engine_platform.h"
#include "Logger.h"





#include "Common/stretchy_buffer.h"
#include "Common/common_macros.h"
#include "debug_macros.h"




void init_asset_importer(AssetImporter* importer, AssetTracker* tracker) {
	arena_init(&importer->mem);
	size_t importer_mem_size = MEGABYTES(200);
	void* start = arena_alloc(&importer->mem, importer_mem_size);
	stack_alloc_init(&importer->stack, start, importer_mem_size);
	importer->stack_allocs_count = 0;
	importer->tracker = tracker;
}

void destroy_asset_importer(AssetImporter* importer) {
	stack_reset(&importer->stack);
	arena_free(&importer->mem);
}

static TextureType fbx_string_texture_type(const char* string) {
	if (strcmp(string, "DiffuseColor") == 0) {
		return TextureType::Albedo;
	} if (strcmp(string, "NormalMap") == 0) {
		return TextureType::Normal;
	} else {
		// TODO: implement rest of texture types
		DEBUG_BREAK;
		return TextureType::None;
	}
}
static AssetID fbx_convert_geo2static_mesh_and_export(AssetImporter* importer, FBX_Geometry* geo, Vec3f pos, Vec3f scale, Vec3f get_rotation, IString path, IString filename){


	// Get the size of the full path
	// + 1 file seperator
	// + 5 _mesh
	// + 1 for null terminator
	u32 str_size = path.length + 1 + 5 + filename.length + ASSET_FILE_EXTENSION_LENGTH + 1;

	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&importer->stack, str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, str_size, "%s_mesh%s", filename.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, str_size);



	AssetID id = track_asset(importer->tracker, file_str, str_size);
	id.type = AssetType::StaticMesh;

	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary

	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Writing to %s,", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Fail writing to %s,", file_str);
	}
	//void* buffer[256];


	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	// Write type of asset
	AssetType type = AssetType::StaticMesh;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	// Write the transform
	fwrite(cast(const void*) &pos, sizeof(pos), 1, file);
	fwrite(cast(const void*) &scale, sizeof(scale), 1, file);
	fwrite(cast(const void*) &get_rotation, sizeof(get_rotation), 1, file);
	
	int vertex_count = geo->index_count * 3;
	int index_count = geo->index_count;

	fwrite(cast(const void*) &vertex_count, sizeof(vertex_count), 1, file);
	fwrite(cast(const void*) &index_count, sizeof(index_count), 1, file);
	
	// TODO: compress output buffers
	//z_stream defstream;
	//defstream.zalloc = Z_NULL;
	//defstream.zfree = Z_NULL;
	//defstream.opaque = Z_NULL;


	//defstream.avail_in = (uInt)mesh->vertex_count * sizeof(Vec3f); // size of uncompressed input buffer
	//defstream.next_in = (Bytef *)mesh->pos; // input char array
	//defstream.avail_out = (uInt)sizeof(b); // size of output
	//defstream.next_out = (Bytef *)b; // output char array


	//deflateInit(&defstream, Z_BEST_COMPRESSION);
	//deflate(&defstream, Z_FINISH);
	//deflateEnd(&defstream);


	// Write the positions straight to the file
	for (int i = 0; i < geo->index_count; i++) {
		fwrite(cast(const void*) &geo->pos[geo->indices[i].x], sizeof(Vec3f), 1, file);
		fwrite(cast(const void*) &geo->pos[geo->indices[i].y], sizeof(Vec3f), 1, file);
		fwrite(cast(const void*) &geo->pos[geo->indices[i].z], sizeof(Vec3f), 1, file);
	}

	// Write indices straight to the file
	for (int i = 0; i < geo->index_count; i++) {
		fwrite(cast(const void*) &Vec3i((3 * i), (3 * i) + 1, (3 * i) + 2), sizeof(Vec3i), 1, file);
	}

	// Write normals straight to file
	for (int i = 0; i < geo->index_count; i++) {

		//fwrite(cast(const void*) &geo->normal[(3 * i) + 0], sizeof(Vec3f), 1, file);
		//fwrite(cast(const void*) &geo->normal[(3 * i) + 1], sizeof(Vec3f), 1, file);
		//fwrite(cast(const void*) &geo->normal[(3 * i) + 2], sizeof(Vec3f), 1, file);

		fwrite(cast(const void*) &geo->normal[0], sizeof(Vec3f), 1, file);
		fwrite(cast(const void*) &geo->normal[1], sizeof(Vec3f), 1, file);
		fwrite(cast(const void*) &geo->normal[2], sizeof(Vec3f), 1, file);
	}

	if (geo->uv_count > 0) {

		for (int i = 0; i < geo->index_count; i++) {

			//fwrite(cast(const void*) &geo->texcoords[geo->uv_indices[i].x], sizeof(Vec2f), 1, file);
			//fwrite(cast(const void*) &geo->texcoords[geo->uv_indices[i].y], sizeof(Vec2f), 1, file);
			//fwrite(cast(const void*) &geo->texcoords[geo->uv_indices[i].z], sizeof(Vec2f), 1, file);

			fwrite(cast(const void*) &geo->texcoords[0], sizeof(Vec2f), 1, file);
			fwrite(cast(const void*) &geo->texcoords[1], sizeof(Vec2f), 1, file);
			fwrite(cast(const void*) &geo->texcoords[2], sizeof(Vec2f), 1, file);
		}
	}



	err = fclose(file);
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Cannot close to %s\n", file_str);
	}



	stack_pop(&importer->stack);

	return id;
}

static AssetID fbx_convert_mat_and_export(AssetImporter* importer, FBX_ImportData* fbx_import, AssetImport_SceneNode* material_node, IString path, IString filename) {


	// Get the size of the full path
	// + 1 file seperator
	// + 4 _mat part
	// + 1 for null terminator
	u32 str_size = path.length + 1 + 4 + filename.length + ASSET_FILE_EXTENSION_LENGTH + 1;

	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&importer->stack, str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, str_size, "%s_mat%s", filename.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, str_size);


	// Track the material asset into our system
	AssetID id = track_asset(importer->tracker, file_str, str_size);
	id.type = AssetType::Material;

	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary

	// Write the material asset to disk
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Writing to %s,", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Fail writing to %s,", file_str);
	}
	//void* buffer[256];


	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	// Write type of asset
	AssetType type = AssetType::Material;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	


	FBX_Material* material = map_get(&fbx_import->fbx_object_map, material_node->id).value.material;

	// NOTE: Conversion between the fbx material format and our own material format
	// since we are using a pbr material, a lot of the data coming from the fbx material will be lost or unused
	// this conversion is more for getting the names and some simple diffuse colors and textures into the engine
	// later on the material author can go into the editor and change the material accordingly
	
	// Write material name length
	fwrite(cast(const void*) &material->name_length, sizeof(material->name_length), 1, file);
	// Write material name 
	fwrite(cast(const void*) material->name, material->name_length, 1, file);


	// Write shading model type
	fwrite(cast(const void*) &material->shading_model, sizeof(material->shading_model), 1, file);

	// Write factors
	// NOTE: Since the fbx format is not using a metallic workflow, most of the data provided in the fbx format is not used
	// however our material importer/exporter expects to see the metallic and roughness factors
	// so when exporting, just use the default values of 0 for the metallic and roughness factors
	float metallic_factor = 0.0f;
	float roughness_factor = 0.0f;
	float emissive_factor = (float)material->emissive_factor;
	fwrite(cast(const void*) &metallic_factor, sizeof(metallic_factor), 1, file);
	fwrite(cast(const void*) &roughness_factor, sizeof(roughness_factor), 1, file);
	fwrite(cast(const void*) &emissive_factor, sizeof(emissive_factor), 1, file);

	// Write colors
	// NOTE: while diffuse and albedo are not really the same, we'll treat them the same here, and then the material author can fix it in the editor
	Vec3f albedo_color = material->diffuse_color;
	fwrite(cast(const void*) &albedo_color, sizeof(albedo_color), 1, file);
	fwrite(cast(const void*) &material->emissive_color, sizeof(material->emissive_color), 1, file);
	
	
	
	
	


	
	
	
	AssetImport_SceneNode* child_texture_node = material_node->first_child;
	
	// Write the texture count
	s32 texture_count = material_node->children_count;
	fwrite(cast(const void*) &texture_count, sizeof(texture_count), 1, file);

	while (child_texture_node != NULL) {
		
		//The child object could either be a texture or a layered texture

		
		FBX_Object obj_type = map_get(&fbx_import->fbx_object_map, child_texture_node->id).value;
		if (obj_type.type == FBX_Object_Type::Texture) {
			// this is a single texture object
			FBX_Texture* t = obj_type.texture;
			

			// Write the texture type
			fwrite(cast(const void*) &t->texture_type, sizeof(t->texture_type), 1, file);

			
			u32 texture_index = child_texture_node->textures[0];
			AssetID texture_id = fbx_import->export_scene.texture_infos[texture_index];
			// Write the texture asset id
			fwrite(cast(const void*) &texture_id, sizeof(texture_id), 1, file);

		} else if (obj_type.type == FBX_Object_Type::LayeredTexture) {
			// TODO: support for layered material
			// this is a layered texture that may have multiple textures that need to be merged/blended
			FBX_LayeredTexture* lt = obj_type.layered_texture;
			
			// Write the texture type
			fwrite(cast(const void*) &lt->texture_type, sizeof(lt->texture_type), 1, file);

			// NOTE: since our material format does not handle layered textures yet, we'll just grab the first texture in the layer
			// For now just grab the first index
			u32 texture_index = child_texture_node->textures[0];
			AssetID texture_id = fbx_import->export_scene.texture_infos[texture_index];
			// Write the texture asset id
			fwrite(cast(const void*) &texture_id, sizeof(texture_id), 1, file);

		}
		
		child_texture_node = child_texture_node->next_sibling;


		
	}
	



	err = fclose(file);
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Cannot close to %s\n", file_str);
	}


	stack_pop(&importer->stack);


	return id;
}


static AssetID fbx_convert_texture_and_export(AssetImporter* importer, FBX_Texture* texture, IString path, IString filename) {

	// Get the size of the full path
	// Get the size of the full path
	// + 1 file seperator
	// + 3 _tx part
	// + 1 for null terminator
	u32 file_str_size = path.length + 1 + 3 + filename.length + ASSET_FILE_EXTENSION_LENGTH + 1;

	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&importer->stack, file_str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, file_str_size, "%s_tx%s", filename.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, file_str_size);

	AssetID id = track_asset(importer->tracker, file_str, file_str_size);
	id.type = AssetType::Texture;

	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary

	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Writing to %s,", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Fail writing to %s,", file_str);
	}
	//void* buffer[256];


	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	// Write type of asset
	AssetType type = AssetType::Texture;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	//// Write name length
	//fwrite(cast(const void*) &texture->name_length, sizeof(texture->name_length), 1, file);
	//// Write name
	//fwrite(cast(const void*) texture->name, texture->name_length, 1, file);
	//
	//// Write filelength
	//fwrite(cast(const void*) &texture->filename_length, sizeof(texture->filename_length), 1, file);
	//// Write file
	//fwrite(cast(const void*) texture->filename, texture->filename_length, 1, file);
	//
	//
	//// Write relative filelength
	//fwrite(cast(const void*) &texture->relative_filename_length, sizeof(texture->relative_filename_length), 1, file);
	//// Write relative file
	//fwrite(cast(const void*) texture->relative_filename, texture->relative_filename_length, 1, file);

	

	
	
	//fwrite(cast(const void*) &texture->translation, sizeof(texture->translation), 1, file);
	fwrite(cast(const void*) &texture->uv_translation, sizeof(texture->uv_translation), 1, file);
	fwrite(cast(const void*) &texture->uv_scaling, sizeof(texture->uv_scaling), 1, file);
	fwrite(cast(const void*) &texture->uv_rotation, sizeof(texture->uv_rotation), 1, file);


	// Write texture payload
	Texture2D txt_tmp;
	txt_tmp.width = 0;
	txt_tmp.height = 0;
	txt_tmp.channels = 0;
	txt_tmp.depth = 0;
	txt_tmp.data = NULL;

	// Read texture file and export back to custom format
	// for now it's simply just a memcpy

	platform_concat_path_and_filename(path, IString(texture->relative_filename, texture->relative_filename_length), file_str, file_str_size);
	
	bool loaded = load_texture(file_str, &txt_tmp, &importer->stack, false);
	


	fwrite(cast(const void*) &txt_tmp.width, sizeof(txt_tmp.width), 1, file);
	fwrite(cast(const void*) &txt_tmp.height, sizeof(txt_tmp.height), 1, file);
	fwrite(cast(const void*) &txt_tmp.channels, sizeof(txt_tmp.channels), 1, file);
	fwrite(cast(const void*) &txt_tmp.depth, sizeof(txt_tmp.depth), 1, file);
	
	s32 txt_size = txt_tmp.width * txt_tmp.height * txt_tmp.channels;
	fwrite(cast(const void*) txt_tmp.data, txt_size, 1, file);
	

	if (loaded) {
		// free texture data from stack
		stack_pop(&importer->stack);
	}
	

	

	
	


	

	
	

	
	err = fclose(file);
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Cannot close to %s\n", file_str);
	}


	stack_pop(&importer->stack);


	return id;
}
AssetID export_static_mesh(AssetImporter* importer, StaticMesh* mesh, Vec3f pos, Vec3f scale, Vec3f get_rotation, IString path, IString filename) {

	
	
	// Get the size of the full path
	// + 1 file seperator
	// + 5 _mesh part
	// + 1 for null terminator
	u32 str_size = path.length + 1 + 5 + filename.length + ASSET_FILE_EXTENSION_LENGTH + 1;

	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&importer->stack, str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, str_size, "%s_mesh%s", filename.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, str_size);

	

	AssetID id = track_asset(importer->tracker, file_str, str_size);
	id.type = AssetType::StaticMesh;

	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary

	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Writing to %s,", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Fail writing to %s,", file_str);
	}
	//void* buffer[256];

	

	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	AssetType type = AssetType::StaticMesh;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	fwrite(cast(const void*) &pos, sizeof(pos), 1, file);
	fwrite(cast(const void*) &scale, sizeof(scale), 1, file);
	fwrite(cast(const void*) &get_rotation, sizeof(get_rotation), 1, file);

	fwrite(cast(const void*) &mesh->vertex_count, sizeof(mesh->vertex_count), 1, file);
	fwrite(cast(const void*) &mesh->index_count, sizeof(mesh->index_count), 1, file);
	//fwrite(cast(const void*) &mesh->normal_count, sizeof(mesh->normal_count), 1, file);
	//fwrite(cast(const void*) &mesh->uv_count, sizeof(mesh->uv_count), 1, file);
	//fwrite(cast(const void*) &mesh->uv_index_count, sizeof(mesh->uv_index_count), 1, file);

	// TODO: compress output buffers
	//z_stream defstream;
	//defstream.zalloc = Z_NULL;
	//defstream.zfree = Z_NULL;
	//defstream.opaque = Z_NULL;


	//defstream.avail_in = (uInt)mesh->vertex_count * sizeof(Vec3f); // size of uncompressed input buffer
	//defstream.next_in = (Bytef *)mesh->pos; // input char array
	//defstream.avail_out = (uInt)sizeof(b); // size of output
	//defstream.next_out = (Bytef *)b; // output char array

	
	//deflateInit(&defstream, Z_BEST_COMPRESSION);
	//deflate(&defstream, Z_FINISH);
	//deflateEnd(&defstream);

	if (mesh->pos) {
		fwrite(cast(const void*) mesh->pos, mesh->vertex_count * sizeof(Vec3f), 1, file);
	}

	if (mesh->indices) {
		fwrite(cast(const void*) mesh->indices, mesh->index_count * sizeof(Vec3i), 1, file);
	}

	if (mesh->normal) {
		fwrite(cast(const void*) mesh->normal, mesh->vertex_count * sizeof(Vec3f), 1, file);
	}

	if (mesh->texcoords) {
		fwrite(cast(const void*) mesh->texcoords, mesh->vertex_count * sizeof(Vec2f), 1, file);
	}

	if (mesh->lightmap_texcoords) {
		//fwrite(cast(const void*) mesh->lightmap_texcoords, mesh->vertex_count * sizeof(Vec2f), 1, file);
	}





	err = fclose(file);
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Cannot close to %s\n", file_str);
	}
		

	stack_pop(&importer->stack);
		
		
	return id;
	
}

static void write_scene_node(AssetImporter* importer, AssetImport_SceneNode* node, FILE* file, bool parent_is_root) {

	

	u32 node_size =
		+ sizeof(node->id)
		+ sizeof(node->name_length)
		+ node->name_length
		+ sizeof(node->translation)
		+ sizeof(node->scale)
		+ sizeof(node->get_rotation)
		+ sizeof(node->children_count)
		+ node->children_count * sizeof(u64)

		+ sizeof(node->mesh_count)
		+ node->mesh_count * sizeof(u32)

		+ sizeof(node->material_count)
		+ node->material_count * sizeof(u32)

		+ sizeof(node->texture_count)
		+ node->texture_count * sizeof(u32);


	// Node size
	fwrite(cast(const void*) &node_size, sizeof(u32), 1, file);
	
	// Node id
	fwrite(cast(const void*) &node->id, sizeof(node->id), 1, file);
	// Length of node name
	fwrite(cast(const void*) &node->name_length, sizeof(node->name_length), 1, file);
	// The node name
	fwrite(cast(const void*) node->name, node->name_length, 1, file);


	// Transform
	Vec3f export_pos = node->translation;
	Vec3f export_rotation = node->get_rotation;
	Vec3f export_scale = node->scale;


	// TODO: better way to check is root without strcmp
	bool is_root = strcmp(node->name, "Root") == 0;
	
	

	
	

	
	export_pos.x = node->translation.data[importer->global_settings.coord_axis];
	export_pos.y = node->translation.data[importer->global_settings.up_axis];
	export_pos.z = node->translation.data[importer->global_settings.front_axis];

	export_scale.x = node->scale.data[importer->global_settings.coord_axis];
	export_scale.y = node->scale.data[importer->global_settings.up_axis];
	export_scale.z = node->scale.data[importer->global_settings.front_axis];

	export_rotation.x = node->get_rotation.data[importer->global_settings.coord_axis];
	export_rotation.y = node->get_rotation.data[importer->global_settings.up_axis];
	export_rotation.z = node->get_rotation.data[importer->global_settings.front_axis];

	

	
	if (parent_is_root) {
		
		//Vec3f axes[3] = { Vec3f_Right, Vec3f_Up, Vec3f_Forward };
		//
		//Vec3f coord_axis = axes[importer->global_settings.coord_axis] * importer->global_settings.coord_axis_sign;
		//Vec3f front_axis = axes[importer->global_settings.front_axis] * importer->global_settings.front_axis_sign;
		//
		//Vec3f test_up_axis = cross(front_axis, coord_axis);
		//Vec3f original_up_axis = axes[ABS(importer->global_settings.original_up_axis)] * copysignf_(1, importer->global_settings.original_up_axis);
		//Vec3f up_axis = axes[importer->global_settings.up_axis] * importer->global_settings.up_axis_sign;

		
		
		//if (test_up_axis == up_axis) {
		//	// This is right hand
		//	// do nothing
		//	//DEBUG_BREAK;
		//} else {
			// left hand to right hand
			//export_pos.x *= -1;
			////DEBUG_BREAK;
			//
			//Mat4x4f rx = rotate(DEG2RAD(export_rotation.x), Vec3f_Right);
			//Mat4x4f ry = rotate(DEG2RAD(export_rotation.y), Vec3f_Up);
			//Mat4x4f rz = rotate(DEG2RAD(export_rotation.z), Vec3f_Forward);
			//
			//Mat4x4f lh = ry * rx * rz;
			//Mat4x4f rh = lh;
			//
			//rh.mat2d[0][2] *= -1;
			//rh.mat2d[1][2] *= -1;
			//rh.mat2d[2][0] *= -1;
			//rh.mat2d[2][1] *= -1;
			//
			//export_rotation = rotation_mat_to_euler(rh);
		//}
		


		// Scale by scale factor
		export_pos = export_pos * importer->global_settings.unit_scale_factor;;
		export_scale = export_scale  * importer->global_settings.unit_scale_factor;
		
		
	}


	if (is_root) {
		parent_is_root = true;
	} else {
		parent_is_root = false;
	}
	
	//debug_print("%s, POS:[%f, %f, %f]\tROT:[%f, %f, %f]\tSCALE:[%f,%f,%f]\n", node->name,
	//	export_pos.x, export_pos.y, export_pos.z,
	//	export_rotation.x, export_rotation.y, export_rotation.z,
	//	export_scale.x, export_scale.y, export_scale.z);
	

	fwrite(cast(const void*) &export_pos, sizeof(node->translation), 1, file);
	fwrite(cast(const void*) &export_scale, sizeof(node->scale), 1, file);
	fwrite(cast(const void*) &export_rotation, sizeof(node->get_rotation), 1, file);
	// amount of children this node has
	fwrite(cast(const void*) &node->children_count, sizeof(node->children_count), 1, file);


	// Write ids of child nodes
	AssetImport_SceneNode* child = node->first_child;
	while (child != NULL) {
		fwrite(cast(const void*) &child->id, sizeof(child->id), 1, file);
		child = child->next_sibling;
	}

	// Mesh count
	fwrite(cast(const void*) &node->mesh_count, sizeof(node->mesh_count), 1, file);
	// texture indices to the scene texture list
	for (u32 j = 0; j < node->mesh_count; j++) {
		fwrite(cast(const void*) &node->meshes[j], sizeof(node->meshes[j]), 1, file);
	}

	// Material count
	fwrite(cast(const void*) &node->material_count, sizeof(node->material_count), 1, file);
	// material indices to the scene texture list
	for (u32 j = 0; j < node->material_count; j++) {
		fwrite(cast(const void*) &node->materials[j], sizeof(node->materials[j]), 1, file);
	}

	// Texture count
	fwrite(cast(const void*) &node->texture_count, sizeof(node->texture_count), 1, file);
	// texture indices to the scene texture list
	for (u32 j = 0; j < node->texture_count; j++) {
		fwrite(cast(const void*) &node->textures[j], sizeof(node->textures[j]), 1, file);
	}



	

	// Recursivly write the node tree
	if (node->first_child != NULL) {
		write_scene_node(importer, node->first_child, file, parent_is_root);
		if (node->first_child->next_sibling != NULL) {
			AssetImport_SceneNode* ptr = node->first_child->next_sibling;
			while (ptr != NULL) {
				write_scene_node(importer, ptr, file, parent_is_root);
				ptr = ptr->next_sibling;
			}
		}
	}

}


AssetID export_asset_scene(AssetImporter* importer, AssetImport_Scene* scene, IString path, IString filename) {
	

	// Get the size of the full path
	// + 1 file seperator
	// + 6 _scene part
	// + 1 for null terminator
	u32 str_size = path.length + 1 + 6 + filename.length + ASSET_FILE_EXTENSION_LENGTH + 1;

	// Alloc on stack to hold the path string
	char* file_str = cast(char*) stack_alloc(&importer->stack, str_size, 1);
	// Generate the file part with the extension
	snprintf(file_str, str_size, "%s_scene%s", filename.buf, ASSET_FILE_EXTENSION);

	IString file_with_ext(file_str);
	platform_concat_path_and_filename(path, file_with_ext, file_str, str_size);



	AssetID id = track_asset(importer->tracker, file_str, str_size);
	id.type = AssetType::Scene;



	



	FILE* file;
	errno_t err;

	err = fopen_s(&file, file_str, "wb");  // write binary

	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Writing to %s,", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Fail writing to %s,", file_str);
	}
	//void* buffer[256];

	// Write asset id
	fwrite(cast(const void*) &id.id, sizeof(id.id), 1, file);

	// Write type
	AssetType type = AssetType::Scene;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	

	fwrite(cast(const void*) &scene->node_count, sizeof(scene->node_count), 1, file);
	fwrite(cast(const void*) &scene->mesh_count, sizeof(scene->mesh_count), 1, file);
	fwrite(cast(const void*) &scene->material_count, sizeof(scene->material_count), 1, file);
	fwrite(cast(const void*) &scene->light_count, sizeof(scene->light_count), 1, file);
	fwrite(cast(const void*) &scene->camera_count, sizeof(scene->camera_count), 1, file);
	fwrite(cast(const void*) &scene->anim_count, sizeof(scene->anim_count), 1, file);
	fwrite(cast(const void*) &scene->texture_count, sizeof(scene->texture_count), 1, file);


	// Write asset ids to file

	for (u32 i = 0; i < scene->mesh_count; i++) {
		AssetID asset = scene->mesh_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	for (u32 i = 0; i < scene->material_count; i++) {
		AssetID asset = scene->material_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	for (u32 i = 0; i < scene->light_count; i++) {
		AssetID asset = scene->light_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	for (u32 i = 0; i < scene->camera_count; i++) {
		AssetID asset = scene->camera_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	for (u32 i = 0; i < scene->anim_count; i++) {
		AssetID asset = scene->animation_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	for (u32 i = 0; i < scene->texture_count; i++) {
		AssetID asset = scene->texture_infos[i];
		fwrite(cast(const void*) &asset.id, sizeof(asset.id), 1, file);
	}

	
	write_scene_node(importer, scene->root, file, false);
	


	err = fclose(file);
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER","Finished writing to %s\n", file_str);
	} else {
		assert_fail();
		LOG_FATAL("FBX_IMPORTER", "Cannot close to %s\n", file_str);
	}

	stack_pop(&importer->stack);




	return id;
}


static bool is_null_node(void* buffer) {
	assert(buffer != NULL);
	char* c = (char*)buffer;
	return c[0] == '\0' &&
		c[1] == '\0' &&
		c[2] == '\0' &&
		c[3] == '\0' &&
		c[4] == '\0' &&
		c[5] == '\0' &&
		c[6] == '\0' &&
		c[7] == '\0' &&
		c[8] == '\0' &&
		c[9] == '\0' &&
		c[10] == '\0' &&
		c[11] == '\0' &&
		c[12] == '\0';
}

static inline int fbx_convert_type_array_char_to_size(char type) {
	switch (type) {
		case 'd': {return sizeof(double);}
		case 'f': {return sizeof(float);}
		case 'l': {return sizeof(s64);}
		case 'i': {return sizeof(s32);}
		case 'b': {return 1;}
		default:  {assert_fail(); return 0; }
	}
}

static inline MaterialShadingModel fbx_shading_string_to_type(const char * string, s32 length) {
	if (strcmp(string, "Phong") == 0) {
		return MaterialShadingModel::Phong;
	} else if( strcmp(string, "lambert") == 0){
		return MaterialShadingModel::Lambert;
	} else {
		return MaterialShadingModel::Unknown;
	}
}

static void fbx_process_material_nodes(AssetImporter* importer, FBX_ImportData* fbx_import) {
	s32 count = sb_count(fbx_import->material_node_cache);

	// Pre alloocate the material infos array
	fbx_import->export_scene.material_count = count;
	sb_add(fbx_import->export_scene.material_infos, count);
	
	IString path(fbx_import->import_path, fbx_import->import_path_length);

	for (int i = 0; i < count; i++) {
		// Get the material node from the cache
		AssetImport_SceneNode* material_node = fbx_import->material_node_cache[i];

		IString mat_name(material_node->name, material_node->name_length);
		// Convert the fbx material to our custom material format
		AssetID material_id = fbx_convert_mat_and_export(importer, fbx_import, material_node, path, mat_name);
		// Add the material id the scene's list of material id
		fbx_import->export_scene.material_infos[i] = material_id;

		// Add the material index to the node's material array
		sb_push(material_node->materials, i);
		material_node->material_count++;
		
	}

}

static void fbx_process_objects_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	FBX_Node* node_to_process = node;

	FBX_Node* obj_node = node_to_process->first_child;

	
	int obj_count = node_to_process->child_count - 1;
	// Loop over all child nodes of the object node
	int geo_index = 0;
	int obj_index = 0;
	while (obj_node != NULL) {

		// Hit a geometry node
		if (strcmp(obj_node->name, "Geometry") == 0) {

			assert(obj_node->properties[0].type_code == 'L');
			u64 id = obj_node->properties[0].primative.L_data;

			FBX_Object object;
			object.type = FBX_Object_Type::Geometry;
			object.geometry = (FBX_Geometry*)stack_alloc(&importer->stack, sizeof(FBX_Geometry), 4);

			
			map_put(&fbx_import->fbx_object_map, id, object);


			object.geometry->vertex_count = 0;
			object.geometry->index_count = 0;
			object.geometry->normal_count = 0;
			object.geometry->uv_count = 0;
			object.geometry->uv_index_count = 0;
			object.geometry->pos = NULL;
			object.geometry->normal = NULL;
			object.geometry->texcoords = NULL;
			object.geometry->indices = NULL;
			object.geometry->uv_indices = NULL;





			FBX_Node* geo_node = obj_node->first_child;
			// Loop over child nodes of the geometry node
			while (geo_node != NULL) {

				if (strcmp(geo_node->name, "Properties60") == 0) {
					FBX_Node* property_node = geo_node->first_child;
					assert_fail();

				} else if (strcmp(geo_node->name, "Properties70") == 0) {
					FBX_Node* property_node = geo_node->first_child;

					while (property_node != NULL) {
						const int num_properties = property_node->num_properties;

						for (int i = 0; i < num_properties; i++) {
							FBX_Property* p = &property_node->properties[i];
							switch (p->type_code) {
								case 'S': {
									//p->special.str_data
									break;
								}

							}

						}

						property_node = property_node->next_sibling;
					}


					// Parse properties
				} else if ((strcmp(geo_node->name, "Vertices") == 0)) {
					// parse vertices
					double* vertices = geo_node->properties->array.d_data;
					const int vertex_count = geo_node->properties->array.array_length / 3;
					object.geometry->pos = NULL;
					object.geometry->vertex_count = vertex_count;
					stb_sb_add(object.geometry->pos, vertex_count);

					for (int i = 0; i < vertex_count; i++) {
						int base_vertex_index = 3 * i;
						object.geometry->pos[i] = Vec3f(vertices[base_vertex_index], vertices[base_vertex_index + 1], vertices[base_vertex_index + 2]);
					}

				} else if ((strcmp(geo_node->name, "PolygonVertexIndex") == 0)) {
					// parse indices
					s32* indices = geo_node->properties->array.i_data;
					const int array_count = geo_node->properties->array.array_length;


					// Since we may have indices that are more than 3,
					// example quads, we cant know for sure what the size of the indices is before hand
					// So we'll make a guess. this will allow us to predefine the size of the array,
					// and it'll grow if there are more indices then estimated
					const int estimated_index_count = array_count / 3;
					int current_index_count = 0;
					// Pre size the array
					object.geometry->indices = NULL;

					// TODO: performance of prefilling
					//sb_add(static_mesh.indices, estimated_index_count);

					// buffer of polygon indices
					int polygon[16] = { 0 };
					// how many indices in this polygon do we have 3 = tri, 4 = quad, etc...
					int polygon_indices_count = 0;

					for (int i = 0; i < array_count; i++) {
						bool has_seen_last_index_in_polygon = false;
						int polygon_index = 0;
						polygon_indices_count = 0;
						while (!has_seen_last_index_in_polygon) {
							polygon_indices_count++;
							int index = indices[i];
							if (index < 0) {
								// indices that are negative mean that this is the last index in the polygon
								// to convert to the actual index value we need to 
								// negate and subtract 1 or faster is to bitwise negate
								index = ~index;
								has_seen_last_index_in_polygon = true;
							} else {
								i++;
							}

							if (polygon_index < 16) {
								polygon[polygon_index] = index;
							}


							polygon_index++;
						}
						switch (polygon_indices_count) {

							// TODO: since we will have multiple models per fbx file, our static mesh wont cut it anymore
							// it may know contain multiple meshes

							case 3: {
								// Simple tri, just copy

								sb_push(object.geometry->indices, Vec3i(polygon[0], polygon[1], polygon[2]));

								current_index_count++;
								break;
							}
							case 4: {
								// Quad to tris

								sb_push(object.geometry->indices, Vec3i(polygon[0], polygon[1], polygon[2]));
								sb_push(object.geometry->indices, Vec3i(polygon[2], polygon[3], polygon[0]));
								current_index_count += 2;

								break;
							}

						}
						// TODO Convert polygon to tri mesh
						//assert(polygon_indices_count <= 4);


					}
					object.geometry->index_count = current_index_count;

				} else if ((strcmp(geo_node->name, "Edges") == 0)) {
					// edges
				} else if ((strcmp(geo_node->name, "LayerElementNormal") == 0)) {
					// parse normals
					FBX_Node* normal_node_children = geo_node->first_child;
					while (normal_node_children != NULL) {

						if (strcmp(normal_node_children->name, "Normals") == 0) {
							// Parse normals

							double* normals = normal_node_children->properties->array.d_data;
							const int normal_count = normal_node_children->properties->array.array_length / 3;
							object.geometry->normal = NULL;
							object.geometry->normal_count = normal_count;

							stb_sb_add(object.geometry->normal, normal_count);

							for (int i = 0; i < normal_count; i++) {
								int base_normal_index = 3 * i;
								object.geometry->normal[i] = Vec3f(normals[base_normal_index], normals[base_normal_index + 1], normals[base_normal_index + 2]);


							}
						}

						normal_node_children = normal_node_children->next_sibling;

					}
				} else if ((strcmp(geo_node->name, "LayerElementMaterial") == 0)) {
					// materials
				}

				else if ((strcmp(geo_node->name, "LayerElementUV") == 0)) {
					// uvs

					FBX_Node* uv_node_child = geo_node->first_child;
					while (uv_node_child != NULL) {

						if (strcmp(uv_node_child->name, "UV") == 0) {
							// Parse normals

							double* uvs = uv_node_child->properties->array.d_data;
							const int uvs_count = uv_node_child->properties->array.array_length / 2;
							object.geometry->uv_count = uvs_count;
							object.geometry->texcoords = NULL;


							stb_sb_add(object.geometry->texcoords, uvs_count);

							for (int i = 0; i < uvs_count; i++) {
								int base_uv_index = 2 * i;

								object.geometry->texcoords[i] = Vec2f(uvs[base_uv_index], uvs[base_uv_index + 1]);


							}

						}

						if (strcmp(uv_node_child->name, "UVIndex") == 0) {
							// Parse normals

							s32* uv_indices = uv_node_child->properties->array.i_data;
							const int uv_indices_count = uv_node_child->properties->array.array_length / 3;
							object.geometry->uv_index_count = uv_indices_count;
							object.geometry->uv_indices = NULL;


							stb_sb_add(object.geometry->uv_indices, uv_indices_count);

							for (int i = 0; i < uv_indices_count; i++) {
								int base_uv_index = 3 * i;

								object.geometry->uv_indices[i] = Vec3i(uv_indices[base_uv_index], uv_indices[base_uv_index + 1], uv_indices[base_uv_index + 2]);
							}

						}

						uv_node_child = uv_node_child->next_sibling;
					}
				}

				else if ((strcmp(geo_node->name, "Layer") == 0)) {

				}


				geo_node = geo_node->next_sibling;
			}
			geo_index++;
		}

		else if (strcmp(obj_node->name, "Model") == 0) {

			assert(obj_node->properties[0].type_code == 'L');
			u64 id = obj_node->properties[0].primative.L_data;

			

			FBX_Object object;
			object.type = FBX_Object_Type::Model;
			object.model = (FBX_Model*)stack_alloc(&importer->stack, sizeof(FBX_Model), 4);

			object.model->name_length = obj_node->properties[1].special.length;
			object.model->name = obj_node->properties[1].special.str_data;
			//strcpy_s(object.model->name, object.model->name_length, obj_node->properties[1].special.str_data);

			// Set defaults, some times, some models dont have the data in the binary, and it assumes tha values are default if not specified
			object.model->local_translation = Vec3f(0, 0, 0);
			object.model->local_rotation = Vec3f(0, 0, 0);
			object.model->local_scaling = Vec3f(1, 1, 1);

			map_put(&fbx_import->fbx_object_map, id, object);

			FBX_Node* model_node = obj_node->first_child;

			// Loop over child nodes of the model node
			while (model_node != NULL) {

				if (strcmp(model_node->name, "Properties70") == 0) {
					FBX_Node* property_node = model_node->first_child;

					while (property_node != NULL) {
						const int num_properties = property_node->num_properties;

						for (int i = 0; i < num_properties; i++) {
							FBX_Property* p = &property_node->properties[i];
							switch (p->type_code) {
								case 'S': {

									if (strcmp(p->special.str_data, "Lcl Translation") == 0) {
										assert(num_properties > 4);
										object.model->local_translation.x = (float)property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_translation.y = (float)property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_translation.z = (float)property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out of loop, cant' use break because we are in switch
									} else if (strcmp(p->special.str_data, "Lcl Rotation") == 0) {
										assert(num_properties > 4);
										object.model->local_rotation.x = (float)property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_rotation.y = (float)property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_rotation.z = (float)property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out of loop, cant' use break because we are in switch
									} else if (strcmp(p->special.str_data, "Lcl Scaling") == 0) {
										assert(num_properties > 4);
										object.model->local_scaling.x = (float)property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_scaling.y = (float)property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_scaling.z = (float)property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out (float)of loop, cant' use break because we are in switch
									}
									break;
								}

							}

						}

						property_node = property_node->next_sibling;
					}
				}
				model_node = model_node->next_sibling;
			}


		}

		else if (strcmp(obj_node->name, "Material") == 0) {
			assert(obj_node->properties[0].type_code == 'L');
			u64 id = obj_node->properties[0].primative.L_data;



			FBX_Object object;
			object.type = FBX_Object_Type::Material;
			object.material = (FBX_Material*)stack_alloc(&importer->stack, sizeof(FBX_Material), 4);

			object.material->name_length = obj_node->properties[1].special.length;
			object.material->name = obj_node->properties[1].special.str_data;

			map_put(&fbx_import->fbx_object_map, id, object);

			FBX_Node* material_node = obj_node->first_child;

			// Loop over child nodes 
			while (material_node != NULL) {

				if (strcmp(material_node->name, "ShadingModel") == 0) {
					object.material->shading_model = fbx_shading_string_to_type(material_node->properties[0].special.str_data, material_node->properties[0].special.length);
				}
				if (strcmp(material_node->name, "Properties70") == 0) {
					FBX_Node* property_node = material_node->first_child;

					while (property_node != NULL) {
						const int num_properties = property_node->num_properties;
						FBX_Property* p = &property_node->properties[0];
						
						if (strcmp(p->special.str_data, "EmissiveColor") == 0) {
							assert(num_properties > 4);
							object.material->emissive_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->emissive_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->emissive_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
							
						}

						else if (strcmp(p->special.str_data, "EmissiveFactor") == 0) {
							object.material->emissive_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "AmbientColor") == 0) {
							assert(num_properties > 4);
							object.material->ambient_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->ambient_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->ambient_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "AmbientFactor") == 0) {
							object.material->ambient_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "DiffuseColor") == 0) {
							assert(num_properties > 4);
							object.material->diffuse_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->diffuse_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->diffuse_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "DiffuseFactor") == 0) {
							object.material->diffuse_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "SpecularColor") == 0) {
							assert(num_properties > 4);
							object.material->specular_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->specular_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->specular_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "SpecularFactor") == 0) {
							object.material->specular_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "TransparentColor") == 0) {
							assert(num_properties > 4);
							object.material->transparent_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->transparent_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->transparent_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "TransparencyFactor") == 0) {
							object.material->transparency_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "ReflectionColor") == 0) {
							assert(num_properties > 4);
							object.material->reflection_color.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.material->reflection_color.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.material->reflection_color.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "ReflectionFactor") == 0) {
							object.material->reflection_factor = property_node->properties[num_properties - 1].primative.D_data;
						}

						else if (strcmp(p->special.str_data, "ShininessExponent") == 0) {
							object.material->shininess_exponent = property_node->properties[num_properties - 1].primative.D_data;
						}
						property_node = property_node->next_sibling;						
					}
				}
				material_node = material_node->next_sibling;
			}
			
		}
		else if (strcmp(obj_node->name, "Texture") == 0) {
		

			assert(obj_node->properties[0].type_code == 'L');
			u64 id = obj_node->properties[0].primative.L_data;



			FBX_Object object;
			object.type = FBX_Object_Type::Texture;
			object.texture = (FBX_Texture*)stack_alloc(&importer->stack, sizeof(FBX_Texture), 4);

			object.texture->name_length = obj_node->properties[1].special.length;
			object.texture->name = obj_node->properties[1].special.str_data;

			map_put(&fbx_import->fbx_object_map, id, object);

			FBX_Node* texture_node = obj_node->first_child;


			// Loop over child nodes 
			while (texture_node != NULL) {

				
				if (strcmp(texture_node->name, "Properties70") == 0) {
					FBX_Node* property_node = texture_node->first_child;

					while (property_node != NULL) {
						const int num_properties = property_node->num_properties;
						FBX_Property* p = &property_node->properties[0];

						
						if (strcmp(p->special.str_data, "Translation") == 0) {
							
							object.texture->translation.x = (float)property_node->properties[num_properties - 3].primative.D_data;
							object.texture->translation.y = (float)property_node->properties[num_properties - 2].primative.D_data;
							object.texture->translation.z = (float)property_node->properties[num_properties - 1].primative.D_data;
						}
						property_node = property_node->next_sibling;
					}

				} else if (strcmp(texture_node->name, "FileName") == 0) {
					object.texture->filename_length = texture_node->properties[0].special.length;
					object.texture->filename = texture_node->properties[0].special.str_data;
				}
				else if (strcmp(texture_node->name, "RelativeFilename") == 0) {
					// NOTE: the provided relative path may not actually be relative
					// so we need to get the base file name ourselves
					const char* basename = platform_file_basename(texture_node->properties[0].special.str_data);
					// cast away const
					object.texture->relative_filename = cast(char*)basename;
					object.texture->relative_filename_length = strlen(object.texture->relative_filename);
				}

				else if (strcmp(texture_node->name, "ModelUVTranslation") == 0) {
					object.texture->uv_translation.x = texture_node->properties[0].primative.D_data;
					object.texture->uv_translation.y = texture_node->properties[1].primative.D_data;
				}

				else if (strcmp(texture_node->name, "ModelUVScaling") == 0) {
					object.texture->uv_scaling.x = texture_node->properties[0].primative.D_data;
					object.texture->uv_scaling.y = texture_node->properties[1].primative.D_data;
				}
				else if (strcmp(texture_node->name, "ModelUVRotation") == 0) {
					DEBUG_BREAK;
				}



				texture_node = texture_node->next_sibling;
			}



			
		}
		else if (strcmp(obj_node->name, "LayeredTexture") == 0) {
			assert(obj_node->properties[0].type_code == 'L');
			u64 id = obj_node->properties[0].primative.L_data;



			FBX_Object object;
			object.type = FBX_Object_Type::LayeredTexture;
			object.layered_texture = (FBX_LayeredTexture*)stack_alloc(&importer->stack, sizeof(FBX_LayeredTexture), 4);

			object.layered_texture->name_length = obj_node->properties[1].special.length;
			object.layered_texture->name = obj_node->properties[1].special.str_data;

			map_put(&fbx_import->fbx_object_map, id, object);

			FBX_Node* layered_texture_node = obj_node->first_child;

			// Loop over child nodes 
			while (layered_texture_node != NULL) {

				if (strcmp(layered_texture_node->name, "LayeredTexture") == 0) {
					FBX_Property* p = layered_texture_node->properties;
					object.layered_texture->layered_texture = p->primative.I_data;

				}

				else if (strcmp(layered_texture_node->name, "BlendModes") == 0) {
					FBX_Property* p = layered_texture_node->properties;
					object.layered_texture->blend_modes = p->primative.I_data;
				}

				else if (strcmp(layered_texture_node->name, "Alphas") == 0) {
					FBX_Property* p = layered_texture_node->properties;
					object.layered_texture->alphas = p->primative.D_data;
				}
				layered_texture_node = layered_texture_node->next_sibling;
			}
		} else {
			//get mats and other objects
			//debug_print("OBJECT TYPE %s\n", obj_node->name);
		}

		


		obj_index++;
		obj_node = obj_node->next_sibling;

	}
}

static void fbx_process_connections_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	FBX_Node* connection_node = node->first_child;

	IString path(fbx_import->import_path, fbx_import->import_path_length);

	while (connection_node != NULL) {
		const int num_properties = connection_node->num_properties;

		char* connection_type = connection_node->properties[0].special.str_data;
		u64 child_object_id = connection_node->properties[1].primative.L_data;
		u64 parent_object_id = connection_node->properties[2].primative.L_data;


		if (strcmp(connection_type, "OO") == 0) {
			// object to object connection	
			assert(num_properties == 3);
			assert(connection_node->properties[0].type_code == 'S');
			assert(connection_node->properties[1].type_code == 'L');
			assert(connection_node->properties[2].type_code == 'L');
			
			
			// Process child node
			MapResult<FBX_Object> child_result = map_get(&fbx_import->fbx_object_map, child_object_id);
			if (!child_result.found) {
				// TODO: This means we havent parsed this object node. it could be a unknown objects our parser doesnt know about
				//assert_fail();
				connection_node = connection_node->next_sibling;
				continue;
			}


			// Process parent node
			AssetImport_SceneNode* parent_scene_node;
			// Root parent
			if (parent_object_id == 0) {
				// Since our map doesnt allow for keys that are zero, we need to check before we get it in the map
				// an id with 0, means it is the root object
				if (fbx_import->export_scene.root == NULL) {
					fbx_import->export_scene.root = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
					
					// Create the root node
					init_scene_node(fbx_import->export_scene.root, parent_object_id, "Root", 4);
					fbx_import->export_scene.node_count++;
				
					set_scene_node_transform(fbx_import->export_scene.root,
						Vec3f_Zero,
						Vec3f(1, 1, 1),
						Vec3f_Zero);
				}
				
				
				parent_scene_node = fbx_import->export_scene.root;
			} else {

				MapResult<FBX_Object> parent_result = map_get(&fbx_import->fbx_object_map, parent_object_id);
				if (!parent_result.found) {
					// TODO: This means we havent parsed this object node. it could be a unknown objects our parser doesnt know about
					//assert_fail();
					connection_node = connection_node->next_sibling;
					continue;
				}
				// this is a parent node, but is not the root node,
				// may be a transitive node(a node that has a child and a parent)
				// A -> B -> C, this is the B node
				
				MapResult<AssetImport_SceneNode*> result = map_get(&fbx_import->scene_node_cache_map, parent_object_id);
				
				// Check to see if we have already seen this node and added to our scene graph
				if (result.found) {
					// This node was already parsed and has been added to our scene graph
					parent_scene_node = result.value;
				} else {

					// At this point we have a child object and parent object


					// This is the first time we seen this object
					// we need to create a scene node for it, and add it to our scene
					parent_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
					

					FBX_Object parent_object = parent_result.value;
					
					

					switch (parent_object.type) {
						case FBX_Object_Type::Model: {
							// There is no transform data on the geometry nodes
							// so we need to go into the parent object which is a model, in order to get the transform data.
							FBX_Model* parent_model = parent_object.model;

							init_scene_node(parent_scene_node, parent_object_id, parent_model->name, parent_model->name_length);
							set_scene_node_transform(fbx_import->export_scene.root,
								parent_model->local_translation,
								parent_model->local_scaling,
								parent_model->local_rotation);
							break;
						}
						case FBX_Object_Type::Material: {
							assert_fail();
							// this should be done in the OP connection type
							break;
						}

						case FBX_Object_Type::LayeredTexture: {
							FBX_LayeredTexture* parent_layered_texture = parent_object.layered_texture;
							init_scene_node(parent_scene_node, parent_object_id, parent_layered_texture->name, parent_layered_texture->name_length);
							break;
						}

					}
					
					fbx_import->export_scene.node_count++;

					// Add it to our cache map, so that next time when we check, we'll know if it has been seen
					map_put(&fbx_import->scene_node_cache_map, parent_object_id, parent_scene_node);
				}
			}


			AssetImport_SceneNode* child_scene_node;
			FBX_Object child_object = child_result.value;
				
			

			
			switch (child_object.type) {
				case FBX_Object_Type::Geometry: {
					
					FBX_Geometry* geo = child_object.geometry;
					
					// Note we should not create a node for this geometry, we simply just attach the geometry to the parent model node
					// Geometry objects dont have any transform/name data attached to them
					// that data is attached to the parent model object
					// So we'll grab the name from the parent in the scene node

					IString parent_name(parent_scene_node->name, parent_scene_node->name_length);
					AssetID asset_id = fbx_convert_geo2static_mesh_and_export(importer,
						geo,
						parent_scene_node->translation,
						parent_scene_node->scale,
						parent_scene_node->get_rotation,
						path,
						parent_name);

					sb_push(fbx_import->export_scene.mesh_infos, asset_id);
					u32 mesh_index = sb_count(fbx_import->export_scene.mesh_infos) - 1;

					
					
					// Since nodes like this can be deeply linked
					sb_push(parent_scene_node->meshes, mesh_index);
					parent_scene_node->mesh_count++;
					fbx_import->export_scene.mesh_count++;
					
					// Free the geo mesh
					stb_sb_free(geo->pos);
					stb_sb_free(geo->indices);
					stb_sb_free(geo->normal);
					stb_sb_free(geo->texcoords);
					stb_sb_free(geo->uv_indices);



					break;
				}
				case FBX_Object_Type::Model: {
					// This is a model which may have may a geometry as a child
					// This may also be a link between a geometry and another parent model
					AssetImport_SceneNode* child_scene_node;

					MapResult<AssetImport_SceneNode*> result = map_get(&fbx_import->scene_node_cache_map, child_object_id);
					if (result.found) {
						child_scene_node = result.value;
					} else {
						child_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
						fbx_import->export_scene.node_count++;
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);
					}

					add_child_to_scene_node(parent_scene_node, child_scene_node);

					init_scene_node(child_scene_node, child_object_id, child_object.model->name, child_object.model->name_length);
					set_scene_node_transform(child_scene_node,
						child_object.model->local_translation,
						child_object.model->local_scaling,
						child_object.model->local_rotation);

					break;
				}
				case FBX_Object_Type::Material: {


					FBX_Material* material = child_object.material;
					MapResult<AssetImport_SceneNode*> result = map_get(&fbx_import->scene_node_cache_map, child_object_id);
					
					if (result.found) {
						child_scene_node = result.value;
					} else {
						child_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
						fbx_import->export_scene.node_count++;
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);

						add_child_to_scene_node(parent_scene_node, child_scene_node);
						init_scene_node(child_scene_node, child_object_id, material->name, material->name_length);

						sb_push(fbx_import->material_node_cache, child_scene_node);
					}

					
					
					
					

					
					break;
				}
				case FBX_Object_Type::Texture: {
					// Add this texture to the parent layered texture
					FBX_Texture* texture = child_object.texture;
					
					

					/*MapResult<AssetImport_SceneNode*> result = map_get(&fbx_import->scene_node_cache_map, child_object_id);

					if (result.found) {
						child_scene_node = result.value;
					} else {
						child_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
						fbx_import->export_scene.node_count++;
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);
					}*/

					//add_child_to_scene_node(parent_scene_node, child_scene_node);

					//init_scene_node(child_scene_node, child_object_id, texture->name, texture->name_length);

					IString tx_name(texture->name, texture->name_length);
					// Create texture asset 
					AssetID asset_id = fbx_convert_texture_and_export(importer, texture, path, tx_name);


					// push the texture to the scene for reference
					sb_push(fbx_import->export_scene.texture_infos, asset_id);
					u32 texture_index = sb_count(fbx_import->export_scene.texture_infos) - 1;
					fbx_import->export_scene.texture_count++;

					// add this asset texture id to the parent node
					parent_scene_node->texture_count++;
					sb_push(parent_scene_node->textures, texture_index);
				
					break;
				}
				case FBX_Object_Type::LayeredTexture: {
					assert_fail();
					// this should be done in the OP connection
					break;
				}
			}

		}

		else if (strcmp(connection_type, "OP") == 0) {

			assert(num_properties == 4);
			assert(connection_node->properties[0].type_code == 'S');
			assert(connection_node->properties[1].type_code == 'L');
			assert(connection_node->properties[2].type_code == 'L');
			assert(connection_node->properties[3].type_code == 'S');

			

			// object to property connection
			MapResult<FBX_Object> parent_result = map_get(&fbx_import->fbx_object_map, parent_object_id);
			if (!parent_result.found) {
				// TODO: This means we havent parsed this object node. it could be a unknown objects our parser doesnt know about
				//assert_fail();
				connection_node = connection_node->next_sibling;
				continue;
			}

			// Process child node
			MapResult<FBX_Object> child_result = map_get(&fbx_import->fbx_object_map, child_object_id);
			if (!child_result.found) {
				// TODO: This means we havent parsed this object node. it could be a unknown objects our parser doesnt know about
				//assert_fail();
				connection_node = connection_node->next_sibling;
				continue;
			}

			// Process parent node
			AssetImport_SceneNode* parent_scene_node;
			MapResult<AssetImport_SceneNode*> result = map_get(&fbx_import->scene_node_cache_map, parent_object_id);
			// Check to see if we have already seen this node and added to our scene graph
			if (result.found) {
				// This node was already parsed and has been added to our scene graph
				parent_scene_node = result.value;
			} else {

				// At this point we have a child object and parent object
				// This is the first time we seen this object
				// we need to create a scene node for it, and add it to our scene
				parent_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
				FBX_Object parent_object = parent_result.value;

				switch (parent_object.type) {
					case FBX_Object_Type::Material: {
						FBX_Material* parent_material = parent_object.material;
						init_scene_node(parent_scene_node, parent_object_id, parent_material->name, parent_material->name_length);
						sb_push(fbx_import->material_node_cache, parent_scene_node);
						break;
					}
				}
			}


			// Process child node
			AssetImport_SceneNode* child_scene_node;
			FBX_Object child_object = child_result.value;




			switch (child_object.type) {
				case FBX_Object_Type::LayeredTexture: {
					
					


					MapResult<AssetImport_SceneNode*> lt_result = map_get(&fbx_import->scene_node_cache_map, child_object_id);


					if (lt_result.found) {
						child_scene_node = lt_result.value;
					} else {
						TextureType texture_type = fbx_string_texture_type(connection_node->properties[3].special.str_data);
						// Add this layered texture to the parent material
						FBX_LayeredTexture* layered_texture = child_object.layered_texture;
						layered_texture->texture_type = texture_type;

						child_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
						fbx_import->export_scene.node_count++;
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);
						init_scene_node(child_scene_node, child_object_id, layered_texture->name, layered_texture->name_length);
					}
					

					add_child_to_scene_node(parent_scene_node, child_scene_node);
					break;
				}
				case FBX_Object_Type::Texture: {
					

					MapResult<AssetImport_SceneNode*> t_result = map_get(&fbx_import->scene_node_cache_map, child_object_id);


					if (t_result.found) {
						child_scene_node = t_result.value;
					} else {
						TextureType texture_type = fbx_string_texture_type(connection_node->properties[3].special.str_data);
						FBX_Texture* texture = child_object.texture;
						texture->texture_type = texture_type;

						child_scene_node = cast(AssetImport_SceneNode*) stack_alloc(&importer->stack, sizeof(AssetImport_SceneNode), 4);
						fbx_import->export_scene.node_count++;
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);
						init_scene_node(child_scene_node, child_object_id, texture->name, texture->name_length);
						add_child_to_scene_node(parent_scene_node, child_scene_node);


						IString tx_name(texture->name, texture->name_length);
						// Create texture asset 
						AssetID asset_id = fbx_convert_texture_and_export(importer, texture, path, tx_name);


						// push the texture to the scene for reference
						sb_push(fbx_import->export_scene.texture_infos, asset_id);
						u32 texture_index = sb_count(fbx_import->export_scene.texture_infos) - 1;
						fbx_import->export_scene.texture_count++;

						// add this asset texture id to the parent node
						child_scene_node->texture_count++;
						sb_push(child_scene_node->textures, texture_index);
					}
					break;
				}
			}

			
		}

		else if (strcmp(connection_type, "PO") == 0) {
			// property to object connection
			DEBUG_BREAK;
		}

		else if (strcmp(connection_type, "PP") == 0) {
			// property to property connection
			DEBUG_BREAK;
		}
		

		

		connection_node = connection_node->next_sibling;
	}

	
}

static void fbx_process_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	
	// Skip nodes that are not objects
	if (strcmp(node->name, "Objects") == 0) {
		fbx_process_objects_node(importer, node, fbx_import);
	} else if (strcmp(node->name, "Connections") == 0) {
		fbx_process_connections_node(importer, node, fbx_import);
	}
	else if (strcmp(node->name, "GlobalSettings") == 0) {
		


		FBX_Node* settings_node = node->first_child;


		while (settings_node != NULL) {

			if (strcmp(settings_node->name, "Properties70") == 0) {
				FBX_Node* property_node = settings_node->first_child;

				while (property_node != NULL) {
					const int num_properties = property_node->num_properties;

					

					for (int i = 0; i < num_properties; i++) {
						FBX_Property* p = &property_node->properties[i];
						switch (p->type_code) {
							case 'S': {

								if (strcmp(p->special.str_data, "UpAxis") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.up_axis = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								} else if (strcmp(p->special.str_data, "UpAxisSign") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.up_axis_sign = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}
								else if (strcmp(p->special.str_data, "FrontAxis") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.front_axis = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}
								else if (strcmp(p->special.str_data, "FrontAxisSign") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.front_axis_sign = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}

								else if (strcmp(p->special.str_data, "CoordAxis") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.coord_axis = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}

								else if (strcmp(p->special.str_data, "CoordAxisSign") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.coord_axis_sign = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}

								else if (strcmp(p->special.str_data, "OriginalUpAxis") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.original_up_axis = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}

								else if (strcmp(p->special.str_data, "OriginalUpAxisSign") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.original_up_axis_sign = property_node->properties[4].primative.I_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}
								else if (strcmp(p->special.str_data, "UnitScaleFactor") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.unit_scale_factor = property_node->properties[4].primative.D_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}

								else if (strcmp(p->special.str_data, "OriginalUnitScaleFactor") == 0) {
									assert(num_properties == 5);
									fbx_import->global_settings.original_unit_scale_factor = property_node->properties[4].primative.D_data;
									i = num_properties; // jump out of loop, cant' use break because we are in switch
								}
								break;
							}

						}

					}

					property_node = property_node->next_sibling;
				}
			}
			settings_node = settings_node->next_sibling;
		}

	}
	
}


static FBX_Node fbx_parse_node(AssetImporter* importer, void* buffer, FILE* file, FBX_ImportData* fbx_import) {
	
	FBX_Node node;
	node.child_count = 0;
	node.first_child = NULL;
	node.next_sibling = NULL;

	long rollback = ftell(file);
	fread(buffer, 4, 1, file);
	node.end_offset = *cast(u32*)buffer;

	if (node.end_offset == 0) {
		fseek(file, rollback, SEEK_SET);
		return node;
	}
	fread(buffer, 4, 1, file);
	node.num_properties = *cast(u32*)buffer;

	fread(buffer, 4, 1, file);
	node.property_list_length = *cast(u32*)buffer;

	fread(buffer, 1, 1, file);
	node.name_length = *cast(u8*)buffer;

	node.name = cast(char*)stack_alloc(&importer->stack, node.name_length + 1, 1);
	fread(node.name, node.name_length, 1, file);
	node.name[node.name_length] = '\0';
	
	importer->stack_allocs_count++;

	


	//if (strcmp(node.name, "FBXHeaderExtension") == 0) {
	//	return node; // skip
	//	
	//} else if (strcmp(node.name, "FileId") == 0) {
	//	return node; // skip
	//}
	//else if (strcmp(node.name, "GlobalSettings") == 0) {
	//	return node; // skip
	//}
	//else if (strcmp(node.name, "Documents") == 0) {
	//	return node; // skip
	//}

	//else if (strcmp(node.name, "References") == 0) {
	//	return node; // skip
	//}

	//else if (strcmp(node.name, "Definitions") == 0) {
	//	return node; // skip
	//}





	int property_count = node.num_properties;
	if (property_count != 0) {

		node.properties = cast(FBX_Property*)stack_alloc(&importer->stack, property_count * sizeof(FBX_Property), 1);
		assert(node.properties != NULL);

		importer->stack_allocs_count++;
		

		for (int i = 0; i < property_count; i++) {
			fread(buffer, 1, 1, file);

			char type = *(char*)buffer;
			node.properties[i].type_code = type;
			switch (type) {
				// Primative type
				case 'Y': {
					fread(buffer, 2, 1, file);
					int16_t value = *cast(int16_t *)buffer;
					node.properties[i].primative.Y_data = value;
					break;
				}
				case 'C': {
					fread(buffer, 1, 1, file);
					char value = *cast(char*)buffer;
					node.properties[i].primative.C_data = value;
					break;
				}

				case 'I': {
					fread(buffer, 4, 1, file);
					s32 value = *cast(s32*)buffer;
					node.properties[i].primative.I_data = value;
					break;
				}

				case 'F': {
					fread(buffer, 4, 1, file);
					float value = *cast(float*)buffer;
					node.properties[i].primative.F_data = value;
					break;
				}

				case 'D': {
					fread(buffer, 8, 1, file);
					double value = *cast(double*)buffer;
					node.properties[i].primative.D_data = value;
					break;
				}
				case 'L': {
					fread(buffer, 8, 1, file);
					s64 value = *cast(s64*)buffer;
					node.properties[i].primative.L_data = value;
					break;
				}
						  // Array types
				case 'd': case 'f': case 'l': case 'i': case 'b': {
					u32 type_size = fbx_convert_type_array_char_to_size(type);

					fread(buffer, 4, 1, file);
					node.properties[i].array.array_length = *cast(u32*)buffer;

					fread(buffer, 4, 1, file);
					u32 encoding = *cast(u32*)buffer;
					assert(encoding == 1 || encoding == 0);
					node.properties[i].array.encoding = encoding;


					fread(buffer, 4, 1, file);
					node.properties[i].array.compressed_length = *cast(u32*)buffer;

					u32 array_size_in_bytes = node.properties[i].array.array_length * type_size;
					switch (encoding) {
						case 0: {

							node.properties[i].array.v_data = stack_alloc(&importer->stack, array_size_in_bytes, 1);
							fread(node.properties[i].array.v_data, array_size_in_bytes, 1, file);
							importer->stack_allocs_count++;
							break;
						}
						case 1: {
							u32 compressed_size_in_bytes = node.properties[i].array.compressed_length;
							z_stream infstream;
							infstream.zalloc = Z_NULL;
							infstream.zfree = Z_NULL;
							infstream.opaque = Z_NULL;

							// Prepare the data array with the expected size
							node.properties[i].array.v_data = stack_alloc(&importer->stack, array_size_in_bytes, 1);
							importer->stack_allocs_count++;

							// Alloc some data to be inflated into
							// once we inflated, we'll pop this off the stack
							void* inflate_buffer = stack_alloc(&importer->stack, compressed_size_in_bytes, 1);

							fread(inflate_buffer, compressed_size_in_bytes, 1, file);
							infstream.avail_in = (uInt)compressed_size_in_bytes;
							infstream.next_in = (Bytef *)inflate_buffer; // input char array
							infstream.avail_out = (uInt)array_size_in_bytes; // size of output
							infstream.next_out = (Bytef *)node.properties[i].array.v_data; // output char array


							inflateInit(&infstream);
							inflate(&infstream, Z_NO_FLUSH);
							inflateEnd(&infstream);
							
							stack_pop(&importer->stack);

							break;

						}
					}
					break;
				}


						  // Special types
				case 'R': {
					fread(buffer, 4, 1, file);
					u32 length = *cast(u32*)buffer;

					node.properties[i].special.length = length;
					node.properties[i].special.raw_binary_data = stack_alloc(&importer->stack, length + 1, 1);
					fread(cast(char*)node.properties[i].special.raw_binary_data, length, 1, file);
					importer->stack_allocs_count++;
					break;
				}

				case 'S': {
					fread(buffer, 4, 1, file);
					u32 length = *cast(u32*)buffer;
					node.properties[i].special.length = length;
					node.properties[i].special.str_data = cast(char*)stack_alloc(&importer->stack, length + 1, 1);
					fread(node.properties[i].special.str_data, length, 1, file);
					node.properties[i].special.str_data[length] = '\0';
					char* debug_str = node.properties[i].special.str_data;
					importer->stack_allocs_count++;
					break;
				}
				default: {
					assert_fail();
					break;
				}


			}


		}

	} else {

	}

	long current_offset = ftell(file);

	
	
	

	while (current_offset < node.end_offset - 13) {
		FBX_Node temp = fbx_parse_node(importer, buffer, file, fbx_import);
		FBX_Node* child_node = cast(FBX_Node*) stack_alloc(&importer->stack, sizeof(temp), 1);
		*child_node = temp;
		importer->stack_allocs_count++;
		

		if (node.child_count == 0) {
			node.first_child = child_node;
		} else {
			FBX_Node* child = node.first_child;
			while (child->next_sibling != NULL) {
				child = child->next_sibling;
			}
			child->next_sibling = child_node;
		}
		node.child_count++;

		current_offset = ftell(file);
	}
	
	// Pass the null record
	rollback = ftell(file);
	fread(buffer, 13, 1, file);
	if (!is_null_node(buffer)) {
		// Roll back
		fseek(file, rollback, SEEK_SET);
	}
	
	



	return node;
}



AssetID import_fbx(AssetImporter* importer, char* filename, bool reimport) {

	
	
	AssetID scene_id;
	AssetID tracked_id = find_asset_by_name(importer->tracker, filename);
	if (tracked_id.id == 0 && tracked_id.type == AssetType::None) {
		// Asset not tracked
		scene_id.id = 0;
		scene_id.type = AssetType::None;
	} else {
		// Asset alredy being tracked
		scene_id = tracked_id;
		// If already being tracked, and we are not reimporting it, then just return what is already imported previously
		if (!reimport) {
			return scene_id;
		}
	}


	
	

	
	

	
	// Init internal fbx import data
	// the data inside fbx import data should be freed at the end of this function call
	FBX_ImportData fbx_import;
	
	fbx_import.material_node_cache = NULL;

	map_init(&fbx_import.fbx_object_map);
	map_init(&fbx_import.scene_node_cache_map);
	// For now we'll default the size to 16
	map_grow(&fbx_import.scene_node_cache_map, 16);

	fbx_import.export_scene.root = NULL;
	fbx_import.export_scene.node_count = 0;

	fbx_import.export_scene.mesh_count = 0;
	fbx_import.export_scene.material_count = 0;
	fbx_import.export_scene.light_count = 0;
	fbx_import.export_scene.camera_count = 0;
	fbx_import.export_scene.anim_count = 0;
	fbx_import.export_scene.texture_count = 0;

	fbx_import.export_scene.mesh_infos = NULL;
	fbx_import.export_scene.material_infos = NULL;
	fbx_import.export_scene.light_infos = NULL;
	fbx_import.export_scene.camera_infos = NULL;
	fbx_import.export_scene.animation_infos = NULL;
	fbx_import.export_scene.texture_infos = NULL;

	
	


	FILE* file;
	errno_t err;

	char path_buffer[260];
	platform_file_dirname(IString(filename), path_buffer, 260);

	IString stripped_filename(platform_file_basename(filename));
	IString path(path_buffer);

	fbx_import.import_path = path.buf;
	fbx_import.import_path_length = path.length;

	// TODO: use memory mapped files
	// TODO: since we have a lot of small strings allocated, we can probably use string interning
	err = fopen_s(&file, filename, "rb");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		LOG_INFO("FBX_IMPORTER", "Opening fbx %s,", filename);
	} else {
		LOG_FATAL("FBX_IMPORTER", "Cannot open fbx%s\n", filename);
		goto cleanup_import_data;
	}


	void* buffer[256];


	// See https://code.blender.org/2013/08/fbx-binary-file-format-specification/
	fread(buffer, 21, 1, file);
	fread(buffer, 2, 1, file);
	fread(buffer, 4, 1, file);
	unsigned int version = *(cast(unsigned int*)buffer);
	if (version < 7100) {
		LOG_WARN("FBX_IMPORTER", "FBX Version %d not supported\n", version);
		if (file) {

			err = fclose(file);
			if (err == 0) {
				LOG_INFO("FBX_IMPORTER", "Closed obj %s\n", filename);
			} else {
				LOG_FATAL("FBX_IMPORTER", "Cannot close obj %s\n", filename);
			}
		}
		goto cleanup_import_data;
	}


	for (;;) {
		//stack_set_reset_point(&importer->stack);
		FBX_Node node = fbx_parse_node(importer, buffer, file, &fbx_import);
		if (node.end_offset == 0) break;
		fbx_process_node(importer, &node, &fbx_import);
		//stack_pop_to_last_reset_point(&importer->stack);
		fseek(file, node.end_offset, SEEK_SET);
	}

	fbx_process_material_nodes(importer, &fbx_import);
	

	// Instead of making a copy, just find the neccessary length to cut of the extension
	//char* stripped_filename = filename;
	//// TODO: implement something more generic
	//// right now it is doing -4 to remove the .fbx extension
	//u32 stripped_filename_length = strlen(filename) - 4;

	
	
	

	importer->global_settings.up_axis = fbx_import.global_settings.up_axis;
	importer->global_settings.up_axis_sign = fbx_import.global_settings.up_axis_sign;
	importer->global_settings.front_axis = fbx_import.global_settings.front_axis;
	importer->global_settings.front_axis_sign = fbx_import.global_settings.front_axis_sign;
	importer->global_settings.coord_axis = fbx_import.global_settings.coord_axis;
	importer->global_settings.coord_axis_sign = fbx_import.global_settings.coord_axis_sign;
	importer->global_settings.original_up_axis = fbx_import.global_settings.original_up_axis;
	importer->global_settings.original_up_axis_sign = fbx_import.global_settings.original_up_axis_sign;
	importer->global_settings.unit_scale_factor = fbx_import.global_settings.unit_scale_factor;
	importer->global_settings.original_unit_scale_factor = fbx_import.global_settings.original_unit_scale_factor;


	//debug_print("\n\nUp axis [%d, %d]\nFront axis[%d, %d]\nCoord Axis[%d, %d]\nOriginal Up Axis[%d, %d]\nScale Factor[%f, %f]\n\n",
	//	importer->global_settings.up_axis, importer->global_settings.up_axis_sign,
	//	importer->global_settings.front_axis, importer->global_settings.front_axis_sign,
	//	importer->global_settings.coord_axis, importer->global_settings.coord_axis_sign,
	//	importer->global_settings.original_up_axis, importer->global_settings.original_up_axis_sign,
	//	importer->global_settings.unit_scale_factor, importer->global_settings.original_unit_scale_factor);
	
	scene_id = export_asset_scene(importer, &fbx_import.export_scene, path, stripped_filename);
	


	
// Clean up memory and any import data
cleanup_import_data:

	
	if (file) {

		err = fclose(file);
		if (err == 0) {
			LOG_INFO("FBX_IMPORTER", "Closed obj %s\n", filename);
		} else {
			LOG_FATAL("FBX_IMPORTER", "Cannot close obj %s\n", filename);
		}
	}

	// We need to free any data from all the nodes
	for (int i = 0; i < fbx_import.scene_node_cache_map.size; i++) {
		CompactMapItem<AssetImport_SceneNode*> node = fbx_import.scene_node_cache_map.map[i];
		if (node.key != 0) {
			sb_free(node.value->meshes);
			sb_free(node.value->textures);
			sb_free(node.value->materials);
		}
	}
	

	sb_free(fbx_import.material_node_cache);

	map_destroy(&fbx_import.fbx_object_map);
	map_destroy(&fbx_import.scene_node_cache_map);

	sb_free(fbx_import.export_scene.mesh_infos);
	sb_free(fbx_import.export_scene.material_infos);
	sb_free(fbx_import.export_scene.light_infos);
	sb_free(fbx_import.export_scene.camera_infos);
	sb_free(fbx_import.export_scene.animation_infos);
	sb_free(fbx_import.export_scene.texture_infos);

	stack_reset(&importer->stack);


	return scene_id;

}





