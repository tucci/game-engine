#pragma once

#include "Asset/FbxImport.h"

#include <stdio.h>
#include <string.h>




#include "Common/stretchy_buffer.h"
#include "Common/common_macros.h"
#include "debug_macros.h"




void init_asset_importer(AssetImporter* importer) {
	arena_init(&importer->mem);
	size_t importer_mem_size = MEGABYTES(100);
	void* start = arena_alloc(&importer->mem, importer_mem_size);
	stack_alloc_init(&importer->stack, start, importer_mem_size);
	importer->stack_allocs_count = 0;

	


}
void destroy_asset_importer(AssetImporter* importer) {
	stack_reset(&importer->stack);
	arena_free(&importer->mem);
	
	
}


AssetInfo export_static_mesh(AssetImporter* importer,
	StaticMesh* mesh,
	Vec3f pos,
	Vec3f rotation,
	Vec3f scale,
	char* filename, int filename_str_len) {

	AssetInfo asset_info;
	asset_info.type = AssetType_StaticMesh;

	size_t str_size = filename_str_len + ASSET_FILE_EXTENSION_LENGTH;
	asset_info.filename = cast(char*)arena_alloc(&importer->mem, str_size);
	snprintf(asset_info.filename, str_size, "%s%s", filename, ASSET_FILE_EXTENSION);


	FILE* file;
	errno_t err;

	err = fopen_s(&file, asset_info.filename, "wb");  // write binary

	if (err == 0) {
		debug_print("Writing to %s,", asset_info.filename);
	} else {
		assert_fail();
		debug_print("Fail writing to %s,", asset_info.filename);
	}
	//void* buffer[256];

	AssetType type = AssetType_StaticMesh;
	fwrite(cast(const void*) &type, sizeof(type), 1, file);

	fwrite(cast(const void*) &pos, sizeof(Vec3f), 1, file);
	fwrite(cast(const void*) &rotation, sizeof(Vec3f), 1, file);
	fwrite(cast(const void*) &scale, sizeof(Vec3f), 1, file);

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
		debug_print("Finished writing to %s\n", asset_info.filename);
	} else {
		assert_fail();
		debug_print("Cannot close to %s\n", asset_info.filename);
	}
		

	
		
		
	return asset_info;
	
}

static bool is_null_node(void* buffer) {
	assert(buffer != NULL);
	char* c = (char*)buffer;
	return c[0] == '\0' &
		c[1] == '\0' &
		c[2] == '\0' &
		c[3] == '\0' &
		c[4] == '\0' &
		c[5] == '\0' &
		c[6] == '\0' &
		c[7] == '\0' &
		c[8] == '\0' &
		c[9] == '\0' &
		c[10] == '\0' &
		c[11] == '\0' &
		c[12] == '\0';
	/*return c[0] == c[1] == c[2] == c[3]
	== c[4] == c[5] == c[6] == c[7]
	== c[8] == c[9] == c[10] == c[11]
	== c[12] == '\0';*/
	//return (strcmp((char*)buffer, FBX_NULL_RECORD) == 0);
}

static inline int fbx_convert_type_array_char_to_size(char type) {
	switch (type) {
		case 'd': {
			return sizeof(double);
			break;
		}
		case 'f': {
			return sizeof(float);
			break;
		}
		case 'l': {
			return sizeof(int64_t);
			break;
		}
		case 'i': {
			return sizeof(int32_t);
			break;
		}
		case 'b': {
			return 1;
			break;
		}
	}
}

static void FBX_geo_to_static_mesh(AssetImporter* importer, FBX_Geometry_Object* geo, StaticMesh* mesh) {
	mesh->vertex_count = 3 * geo->index_count;
	mesh->index_count = geo->index_count;

	mesh->pos = cast(Vec3f*)stack_alloc(&importer->stack, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->indices = cast(Vec3i*)stack_alloc(&importer->stack, mesh->index_count * sizeof(Vec3i), 4);
	mesh->normal = cast(Vec3f*)stack_alloc(&importer->stack, mesh->vertex_count * sizeof(Vec3f), 4);
	mesh->texcoords = cast(Vec2f*)stack_alloc(&importer->stack, mesh->vertex_count * sizeof(Vec2f), 4);



	for (int i = 0; i < mesh->index_count; i++) {
		// TODO: implement y is up
		mesh->pos[(3 * i) + 0] = geo->pos[geo->indices[i].x];
		mesh->pos[(3 * i) + 1] = geo->pos[geo->indices[i].y];
		mesh->pos[(3 * i) + 2] = geo->pos[geo->indices[i].z];

		mesh->indices[i] = Vec3i((3 * i), (3 * i) + 1, (3 * i) + 2);

		//mesh->normal[(3 * i) + 0] = geo->normal[(3 * i) + 0];
		//mesh->normal[(3 * i) + 1] = geo->normal[(3 * i) + 1];
		//mesh->normal[(3 * i) + 2] = geo->normal[(3 * i) + 2];

		mesh->normal[(3 * i) + 0] = geo->normal[0];
		mesh->normal[(3 * i) + 1] = geo->normal[1];
		mesh->normal[(3 * i) + 2] = geo->normal[2];




		//mesh->texcoords[(3 * i) + 0] = geo->texcoords[geo->uv_indices[i].x];
		//mesh->texcoords[(3 * i) + 1] = geo->texcoords[geo->uv_indices[i].y];
		//mesh->texcoords[(3 * i) + 2] = geo->texcoords[geo->uv_indices[i].z];
		//
		mesh->texcoords[(3 * i) + 0] = geo->texcoords[0];
		mesh->texcoords[(3 * i) + 1] = geo->texcoords[1];
		mesh->texcoords[(3 * i) + 2] = geo->texcoords[2];

	}

}



void static fbx_process_objects_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	FBX_Node* node_to_process = node;

	FBX_Node* node_attr = node_to_process->first_child;
	FBX_Node* obj_node = node_attr->next_sibling;

	int obj_count = node_to_process->child_count - 1;
	// Loop over all child nodes of the object node
	int geo_index = 0;
	int obj_index = 0;
	while (obj_node != NULL) {

		// Hit a geometry node
		if (strcmp(obj_node->name, "Geometry") == 0) {

			assert(obj_node->properties[0].type_code == 'L');
			uint64_t id = obj_node->properties[0].primative.L_data;

			FBX_Object object;
			object.type = FBX_Object_Type_Geometry;
			object.geo = (FBX_Geometry_Object*)stack_alloc(&importer->stack, sizeof(FBX_Geometry_Object), 4);

			
			map_put(&fbx_import->fbx_object_map, id, object);


			object.geo->vertex_count = 0;
			object.geo->index_count = 0;
			object.geo->normal_count = 0;
			object.geo->uv_count = 0;
			object.geo->uv_index_count = 0;
			object.geo->pos = NULL;
			object.geo->normal = NULL;
			object.geo->texcoords = NULL;
			object.geo->indices = NULL;
			object.geo->uv_indices = NULL;





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
					object.geo->pos = NULL;
					object.geo->vertex_count = vertex_count;
					stb_sb_add(object.geo->pos, vertex_count);

					for (int i = 0; i < vertex_count; i++) {
						int base_vertex_index = 3 * i;
						object.geo->pos[i] = Vec3f(vertices[base_vertex_index], vertices[base_vertex_index + 1], vertices[base_vertex_index + 2]);
					}

				} else if ((strcmp(geo_node->name, "PolygonVertexIndex") == 0)) {
					// parse indices
					int32_t* indices = geo_node->properties->array.i_data;
					const int array_count = geo_node->properties->array.array_length;


					// Since we may have indices that are more than 3,
					// example quads, we cant know for sure what the size of the indices is before hand
					// So we'll make a guess. this will allow us to predefine the size of the array,
					// and it'll grow if there are more indices then estimated
					const int estimated_index_count = array_count / 3;
					int current_index_count = 0;
					// Pre size the array
					object.geo->indices = NULL;

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

								sb_push(object.geo->indices, Vec3i(polygon[0], polygon[1], polygon[2]));

								current_index_count++;
								break;
							}
							case 4: {
								// Quad to tris

								sb_push(object.geo->indices, Vec3i(polygon[0], polygon[1], polygon[2]));
								sb_push(object.geo->indices, Vec3i(polygon[2], polygon[3], polygon[0]));
								current_index_count += 2;

								break;
							}

						}
						// TODO Convert polygon to tri mesh
						//assert(polygon_indices_count <= 4);


					}
					object.geo->index_count = current_index_count;

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
							object.geo->normal = NULL;
							object.geo->normal_count = normal_count;

							stb_sb_add(object.geo->normal, normal_count);

							for (int i = 0; i < normal_count; i++) {
								int base_normal_index = 3 * i;
								object.geo->normal[i] = Vec3f(normals[base_normal_index], normals[base_normal_index + 1], normals[base_normal_index + 2]);


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
							object.geo->uv_count = uvs_count;
							object.geo->texcoords = NULL;


							stb_sb_add(object.geo->texcoords, uvs_count);

							for (int i = 0; i < uvs_count; i++) {
								int base_uv_index = 2 * i;

								object.geo->texcoords[i] = Vec2f(uvs[base_uv_index], uvs[base_uv_index + 1]);


							}

						}

						if (strcmp(uv_node_child->name, "UVIndex") == 0) {
							// Parse normals

							int32_t* uv_indices = uv_node_child->properties->array.i_data;
							const int uv_indices_count = uv_node_child->properties->array.array_length / 3;
							object.geo->uv_index_count = uv_indices_count;
							object.geo->uv_indices = NULL;


							stb_sb_add(object.geo->uv_indices, uv_indices_count);

							for (int i = 0; i < uv_indices_count; i++) {
								int base_uv_index = 3 * i;

								object.geo->uv_indices[i] = Vec3i(uv_indices[base_uv_index], uv_indices[base_uv_index + 1], uv_indices[base_uv_index + 2]);
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
			uint64_t id = obj_node->properties[0].primative.L_data;

			

			FBX_Object object;
			object.type = FBX_Object_Type_Model;
			object.model = (FBX_Model_Object*)stack_alloc(&importer->stack, sizeof(FBX_Model_Object), 4);

			object.model->name_length = obj_node->properties[1].special.length;
			object.model->name = obj_node->properties[1].special.str_data;
			//strcpy_s(object.model->name, object.model->name_length, obj_node->properties[1].special.str_data);

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
										object.model->local_translation.x = property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_translation.y = property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_translation.z = property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out of loop, cant' use break because we are in switch
									} else if (strcmp(p->special.str_data, "Lcl Rotation") == 0) {
										assert(num_properties > 4);
										object.model->local_rotation.x = property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_rotation.y = property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_rotation.z = property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out of loop, cant' use break because we are in switch
									} else if (strcmp(p->special.str_data, "Lcl Scaling") == 0) {
										assert(num_properties > 4);
										object.model->local_scaling.x = property_node->properties[num_properties - 3].primative.D_data;
										object.model->local_scaling.y = property_node->properties[num_properties - 2].primative.D_data;
										object.model->local_scaling.z = property_node->properties[num_properties - 1].primative.D_data;
										i = num_properties; // jump out of loop, cant' use break because we are in switch
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
		obj_index++;
		obj_node = obj_node->next_sibling;

	}
}

void static fbx_process_connections_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	FBX_Node* connection_node = node->first_child;

	while (connection_node != NULL) {
		const int num_properties = connection_node->num_properties;

		char* connection_type = connection_node->properties[0].special.str_data;
		uint64_t child_object_id = connection_node->properties[1].primative.L_data;
		uint64_t parent_object_id = connection_node->properties[2].primative.L_data;

		if (strcmp(connection_type, "OO") == 0) {
			// object to object connection
			assert(num_properties == 3);

			assert(connection_node->properties[0].type_code == 'S');
			assert(connection_node->properties[1].type_code == 'L');
			assert(connection_node->properties[2].type_code == 'L');
			
			MapResult<FBX_Object> child_result = map_get(&fbx_import->fbx_object_map, child_object_id);
			if (!child_result.found) {
				// TODO: This means we havent parsed this object node. it could be a material
				//assert_fail();
				connection_node = connection_node->next_sibling;
				continue;
			}

			
			FBX_Object child_object = child_result.value;
				
			

			AssetSceneNode* parent_scene_node;

			// Root parent
			if (parent_object_id == 0) {
				// Since our map doesnt allow for keys that are zero, we need to check before we get it in the map
				// an id with 0, means it is the root object
				if (fbx_import->export_scene.root == NULL) {
					fbx_import->export_scene.root = cast(AssetSceneNode*) stack_alloc(&importer->stack, sizeof(AssetSceneNode), 4);
					init_scene_node(fbx_import->export_scene.root, "Root", 4);
					set_scene_node_transform(fbx_import->export_scene.root,
						Vec3f(0, 0, 0),
						Vec3f(1, 1, 1),
						Vec3f(0, 0, 0));
				}
				
				
				parent_scene_node = fbx_import->export_scene.root;



			} else {
				// is a parent node, but is not the root node,
				// may be a transitive node(a node that has a child and a parent)
				// A -> B -> C, this is the B node
				MapResult<FBX_Object> parent_result = map_get(&fbx_import->fbx_object_map, parent_object_id);
				
				if (!parent_result.found) {
					assert_fail();
					continue;
				}

				

				
				MapResult<AssetSceneNode*> result = map_get(&fbx_import->scene_node_cache_map, parent_object_id);
				
				// Check to see if we have already seen this node and added to our scene graph
				if (result.found) {
					// This node was already parsed and has been added to our scene graph
					parent_scene_node = result.value;
				} else {


					FBX_Object parent_object = parent_result.value;
					// At this point we have a child object and parent object
					assert(parent_object.type == FBX_Object_Type_Model);
					// There is no transform data on the geometry nodes
					// so we need to go into the parent object which is a model, in order to get the transform data.
					FBX_Model_Object* parent_model = parent_object.model;


					// This is the first time we seen this object
					// we need to create a scene node for it, and add it to our scene
					parent_scene_node = cast(AssetSceneNode*) stack_alloc(&importer->stack, sizeof(AssetSceneNode), 4);
					// Create the node, and add the transform
					init_scene_node(parent_scene_node, parent_model->name, parent_model->name_length);
					set_scene_node_transform(fbx_import->export_scene.root,
						parent_model->local_translation,
						parent_model->local_scaling,
						parent_model->local_rotation);

					// Add it to our cache map, so that next time when we check, we'll know if it has been seen
					map_put(&fbx_import->scene_node_cache_map, parent_object_id, parent_scene_node);
				}
				

				

				
				

			}


			
				
			


			switch (child_object.type) {
				case FBX_Object_Type_Geometry: {

					FBX_Geometry_Object* geo = child_object.geo;
					// Geometry nodes dont have any transform/name data attached to them
					// that data is attached to the parent model object
					// So we'll grab the name from the parent in the scene node
					


					

					

					StaticMesh static_mesh;

					to avoid copying, we can just write straight to the file, and avoid the extra copy allocs
					FBX_geo_to_static_mesh(importer, geo, &static_mesh);

					Here are the current problems
					- We cant just set the address of static_mesh and add it to our export_scene, bc it is local
					- when we do FBX_geo_to_static_mesh is rearranges the mesh data, and stores it, but the problem is that it makes a copy of all that data.
						so while we are parsing, it will take double the memory, which is bad

					Possible Solutions
						- we build the entire export tree with all the duplicated meshes, and then write the scene file
						- as we build the scene, we write to our scene file, but the problem is how do u manage multiple links
						
					

					fbx_import->export_scene.meshes[fbx_import->export_scene.mesh_count] = 
					
					

					parent_scene_node->meshes[parent_scene_node->mesh_count] = fbx_import->export_scene.mesh_count;
					parent_scene_node->mesh_count++;

					fbx_import->export_scene.mesh_count++;
						

					

					

					




					//if (geo->uv_count > 0) {

					//	StaticMesh static_mesh;
					//	FBX_geo_to_static_mesh(importer, geo, &static_mesh);

					//	//need to export with transform, but also we need to think about exporting the parent models and all this
					//	//i guess we could create an fbx importer that has a root fbx scene, we parse into that, then at the end, once the tree is made
					//	//we traverse it and export that way?


					//	AssetInfo asset_info = export_static_mesh(importer, &static_mesh,
					//		local_pos,
					//		local_rot,
					//		local_scale,
					//		name, str_length);

					//	stack_pop(&importer->stack);
					//	stack_pop(&importer->stack);
					//	stack_pop(&importer->stack);
					//	stack_pop(&importer->stack);
					//}

					// Free the geo mesh
					stb_sb_free(geo->pos);
					stb_sb_free(geo->indices);
					stb_sb_free(geo->normal);
					stb_sb_free(geo->texcoords);
					stb_sb_free(geo->uv_indices);



					break;
				}

				case FBX_Object_Type_Model: {
					// This is a model which may have may a geometry as a child
					// This may also be a link between a geometry and another parent model
					AssetSceneNode* child_scene_node;

					MapResult<AssetSceneNode*> result = map_get(&fbx_import->scene_node_cache_map, child_object_id);
					if (result.found) {
						child_scene_node = result.value;
					} else {
						child_scene_node = cast(AssetSceneNode*) stack_alloc(&importer->stack, sizeof(AssetSceneNode), 4);
						map_put(&fbx_import->scene_node_cache_map, child_object_id, child_scene_node);
					}

					add_child_to_scene_node(parent_scene_node, child_scene_node);

					init_scene_node(child_scene_node, child_object.model->name, child_object.model->name_length);
					set_scene_node_transform(child_scene_node,
						child_object.model->local_translation,
						child_object.model->local_scaling,
						child_object.model->local_rotation);

					

					
					break;
				}
			}

		}

		else if (strcmp(connection_type, "OP") == 0) {
			// object to property connection
			//DEBUG_BREAK;
		}

		else if (strcmp(connection_type, "PO") == 0) {
			// property to object connection
			//DEBUG_BREAK;
		}

		else if (strcmp(connection_type, "PP") == 0) {
			// property to property connection
			//DEBUG_BREAK;
		}
		

		

		connection_node = connection_node->next_sibling;
	}

	
}


void static fbx_process_node(AssetImporter* importer, FBX_Node* node, FBX_ImportData* fbx_import) {
	
	// Skip nodes that are not objects
	if (strcmp(node->name, "Objects") == 0) {
		fbx_process_objects_node(importer, node, fbx_import);
	} else if (strcmp(node->name, "Connections") == 0) {
		fbx_process_connections_node(importer, node, fbx_import);
	}


	
}


static FBX_Node fbx_parse_node(AssetImporter* importer, void* buffer, FILE* file, FBX_ImportData* fbx_import) {
	
	FBX_Node node;
	node.child_count = 0;
	node.first_child = NULL;
	node.next_sibling = NULL;

	long rollback = ftell(file);
	fread(buffer, 4, 1, file);
	node.end_offset = *cast(uint32_t*)buffer;

	if (node.end_offset == 0) {
		fseek(file, rollback, SEEK_SET);
		return node;
	}
	fread(buffer, 4, 1, file);
	node.num_properties = *cast(uint32_t*)buffer;

	fread(buffer, 4, 1, file);
	node.property_list_length = *cast(uint32_t*)buffer;

	fread(buffer, 1, 1, file);
	node.name_length = *cast(uint8_t*)buffer;

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
					int32_t value = *cast(int32_t*)buffer;
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
					int64_t value = *cast(int64_t*)buffer;
					node.properties[i].primative.L_data = value;
					break;
				}
						  // Array types
				case 'd': case 'f': case 'l': case 'i': case 'b': {
					uint32_t type_size = fbx_convert_type_array_char_to_size(type);

					fread(buffer, 4, 1, file);
					node.properties[i].array.array_length = *cast(uint32_t*)buffer;

					fread(buffer, 4, 1, file);
					uint32_t encoding = *cast(uint32_t*)buffer;
					assert(encoding == 1 || encoding == 0);
					node.properties[i].array.encoding = encoding;


					fread(buffer, 4, 1, file);
					node.properties[i].array.compressed_length = *cast(uint32_t*)buffer;

					uint32_t array_size_in_bytes = node.properties[i].array.array_length * type_size;
					switch (encoding) {
						case 0: {

							node.properties[i].array.v_data = stack_alloc(&importer->stack, array_size_in_bytes, 1);
							fread(node.properties[i].array.v_data, array_size_in_bytes, 1, file);
							importer->stack_allocs_count++;
							break;
						}
						case 1: {
							uint32_t compressed_size_in_bytes = node.properties[i].array.compressed_length;
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
					uint32_t length = *cast(uint32_t*)buffer;

					node.properties[i].special.length = length;
					node.properties[i].special.raw_binary_data = stack_alloc(&importer->stack, length + 1, 1);
					fread(cast(char*)node.properties[i].special.raw_binary_data, length, 1, file);
					importer->stack_allocs_count++;
					break;
				}

				case 'S': {
					fread(buffer, 4, 1, file);
					uint32_t length = *cast(uint32_t*)buffer;
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



AssetInfo import_fbx(AssetImporter* importer, const char* filename, bool y_is_up) {
	
	AssetInfo export_scene_file;
	export_scene_file.type = AssetType_None;
	export_scene_file.filename = NULL;

	
	// Init internal fbx import data
	// the data inside fbx import data should be freed at the end of this function call
	FBX_ImportData fbx_import;
	fbx_import.y_is_up = y_is_up;
	map_init(&fbx_import.fbx_object_map);
	map_init(&fbx_import.scene_node_cache_map);
	// For now we'll default the size to 16
	map_grow(&fbx_import.scene_node_cache_map, 16);

	
	fbx_import.export_scene.root = NULL;
	fbx_import.export_scene.mesh_count = 0;
	fbx_import.export_scene.meshes = NULL;
	



	FILE* file;
	errno_t err;

	// TODO: use memory mapped files
	// TODO: since we have a lot of small strings allocated, we can probably use string interning
	err = fopen_s(&file, filename, "rb");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		debug_print("Opening fbx %s,", filename);
	} else {
		debug_print("Cannot open fbx%s\n", filename);
		goto cleanup_import_data;
	}


	void* buffer[256];


	// See https://code.blender.org/2013/08/fbx-binary-file-format-specification/
	fread(buffer, 21, 1, file);
	fread(buffer, 2, 1, file);
	fread(buffer, 4, 1, file);
	unsigned int version = *(cast(unsigned int*)buffer);
	if (version < 7100) {
		debug_print("FBX Version %d not supported\n", version);
		if (file) {

			err = fclose(file);
			if (err == 0) {
				debug_print("Closed obj %s\n", filename);
			} else {
				debug_print("Cannot close obj %s\n", filename);
			}
		}
	}



	


	
	export_scene_file.type = AssetType_Scene;

	// Reserve space for the export scene file
	size_t str_size = strlen(filename) + ASSET_FILE_EXTENSION_LENGTH;
	export_scene_file.filename = cast(char*)arena_alloc(&importer->mem, str_size);
	snprintf(export_scene_file.filename, str_size, "%s%s", filename, ASSET_FILE_EXTENSION);

	
	
	



	for (;;) {
		//stack_set_reset_point(&importer->stack);
		FBX_Node node = fbx_parse_node(importer, buffer, file, &fbx_import);
		if (node.end_offset == 0) break;
		fbx_process_node(importer, &node, &fbx_import);
		//stack_pop_to_last_reset_point(&importer->stack);
		fseek(file, node.end_offset, SEEK_SET);
	}



	


	
// Label
cleanup_import_data:

	if (file) {

		err = fclose(file);
		if (err == 0) {
			debug_print("Closed obj %s\n", filename);
		} else {
			debug_print("Cannot close obj %s\n", filename);
		}
	}

	

	// Clean up memory and any import data

	stack_reset(&importer->stack);


	map_destroy(&fbx_import.fbx_object_map);
	map_destroy(&fbx_import.scene_node_cache_map);
	

	return export_scene_file;

}



