#pragma once


#include "Core/ECS/Component/StaticMesh.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>


#include "zlib.h"


#include "debug_macros.h"

#include "Common/common_macros.h"
#include "Common/stretchy_buffer.h"




static float parse_float(char** stream) {
	const char* start = *stream;
	if (**stream == '-') {
		(*stream)++;
	}
	while (isdigit(**stream)) {
		(*stream)++;
	}

	if (**stream == '.') {
		(*stream)++;
	}

	while (isdigit(**stream)) {
		(*stream)++;
	}
	float f = strtof((start), NULL);
	return f;
}

static int parse_int(char** stream) {
	const char* start = *stream;
	if (**stream == '-') {
		(*stream)++;
	}

	while (isdigit(**stream)) {
		(*stream)++;
	}

	int i = strtol(start, NULL, 10);
	return i;

}

static void skip_spaces(char** stream) {
	while (**stream == ' ') { (*stream)++; }// skip spaces
}





bool obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, StackAllocator* memory) {

	// Load data into obj model first
	ObjModel model;

	model.verts = 0;
	model.texcoords = 0;
	model.normals = 0;
	model.v_id = 0;
	model.vt_id = 0;
	model.vn_id = 0;







	FILE* file;
	errno_t err;

	err = fopen_s(&file, filename, "r");

	// TODO: if we are not able to load the file, our engine should handle it properly
	if (err == 0) {
		
		debug_print("Opening obj %s,", filename);
		
	} else {
		debug_print("Cannot open obj %s\n", filename);
		return false;
	}

	enum {
		LINE_SIZE = 256
	};

	char line[LINE_SIZE];

	while (fgets(line, sizeof(line), file)) {
		char* stream = line;
		Vec4f v;
		Vec3i v_id;
		Vec3i vt_id;
		Vec3i vn_id;


		bool has_vts = false;
		bool has_vns = false;
		switch (*stream) {
			case 'v':
				stream++;
				switch (*stream) {
					case ' ': // Positions
						skip_spaces(&stream);
						v.x = parse_float(&stream);
						skip_spaces(&stream);
						v.y = parse_float(&stream);
						skip_spaces(&stream);
						v.z = parse_float(&stream);
						v.w = 1;
						stb_sb_push(model.verts, v);
						break;
					case 't': // Textures
						stream++;
						skip_spaces(&stream);
						v.x = parse_float(&stream);
						skip_spaces(&stream);
						v.y = parse_float(&stream);
						skip_spaces(&stream);
						v.z = parse_float(&stream);
						stb_sb_push(model.texcoords, v.xyz.xy);
						break;
					case 'n': // Normals
						stream++;
						skip_spaces(&stream);
						v.x = parse_float(&stream);
						skip_spaces(&stream);
						v.y = parse_float(&stream);
						skip_spaces(&stream);
						v.z = parse_float(&stream);
						stb_sb_push(model.normals, v.xyz);
						break;
					default:
						break;
				}

				break;
			case 'f':
				stream++;
				for (int i = 0; i < 3; i++) {
					while (*stream == ' ') { stream++; }// skip spaces
					v_id.data[i] = parse_int(&stream) - 1;
					if (*stream == '/') {
						has_vts = true;
						stream++;
						vt_id.data[i] = parse_int(&stream) - 1;
					}
					if (*stream == '/') {
						has_vns = true;
						stream++;
						vn_id.data[i] = parse_int(&stream) - 1;
					}
				}

				stb_sb_push(model.v_id, v_id);
				if (has_vts) {
					stb_sb_push(model.vt_id, vt_id);
				}
				if (has_vns) {
					stb_sb_push(model.vn_id, vn_id);
				}


				break;
			default:
				break;
		}

	}

	

	if (file) {
		err = fclose(file);
		if (err == 0) {
			debug_print("Closed obj %s\n", filename);
		} else {
			debug_print("Cannot close obj %s\n", filename);
		}
	}

	debug_print("Vertex count %d, ", stb_sb_count(model.verts));
	debug_print("Face count %d\n", stb_sb_count(model.v_id));

	model.vert_count = stb_sb_count(model.verts);
	model.texcoord_count = stb_sb_count(model.texcoords);
	model.normal_count = stb_sb_count(model.normals);
	model.face_count = stb_sb_count(model.v_id);
	model.tex_indice_count = stb_sb_count(model.vt_id);



	
	// Copy and Convert our model into our static mesh
	int index_count = model.face_count;
	static_mesh->index_count = index_count;
	static_mesh->vertex_count = index_count * 3;

	void* mem_block = stack_alloc(memory,
		  static_mesh->index_count * sizeof(Vec3i)  // indices
		+ static_mesh->vertex_count * sizeof(Vec3f) // pos
		+ static_mesh->vertex_count * sizeof(Vec3f) // normals
		+ static_mesh->vertex_count * sizeof(Vec2f) // texcoords
		, 4);

	if (mem_block) {
		static_mesh->indices = cast(Vec3i*) mem_block;
		static_mesh->pos = cast(Vec3f*) (cast(char*)mem_block + (static_mesh->index_count * sizeof(Vec3i))); // + offset of indices block
		static_mesh->normal = cast(Vec3f*)(cast(char*)static_mesh->pos + (static_mesh->vertex_count * sizeof(Vec3f))); // + offset of pos block
		static_mesh->texcoords = cast(Vec2f*)(cast(char*)static_mesh->normal + (static_mesh->vertex_count * sizeof(Vec3f))); // + offset of normal block
	
		for (int i = 0; i < index_count; i++) {
		
			static_mesh->indices[i] = Vec3i( 3 * i, 3 * i + 1, 3 * i + 2 );
			Vec3i uv =     model.vt_id[i];
			Vec3i face =   model.v_id[i];
			Vec3i normal = model.vn_id[i];


			static_mesh->pos[3 * i + 0] = model.verts[face.x].xyz;
			static_mesh->pos[3 * i + 1] = model.verts[face.y].xyz;
			static_mesh->pos[3 * i + 2] = model.verts[face.z].xyz;

			static_mesh->texcoords[3 * i + 0] = model.texcoords[uv.x];
			static_mesh->texcoords[3 * i + 1] = model.texcoords[uv.y];
			static_mesh->texcoords[3 * i + 2] = model.texcoords[uv.z];

			static_mesh->normal[3 * i + 0] = model.normals[normal.x];
			static_mesh->normal[3 * i + 1] = model.normals[normal.y];
			static_mesh->normal[3 * i + 2] = model.normals[normal.z];

		}
		// Free temp obj model
		stb_sb_free(model.verts);
		stb_sb_free(model.texcoords);
		stb_sb_free(model.normals);

		stb_sb_free(model.v_id);
		stb_sb_free(model.vt_id);
		stb_sb_free(model.vn_id);
		return true;
	} else {
		// Memory cannot be taken
		// Free temp obj model
		stb_sb_free(model.verts);
		stb_sb_free(model.texcoords);
		stb_sb_free(model.normals);

		stb_sb_free(model.v_id);
		stb_sb_free(model.vt_id);
		stb_sb_free(model.vn_id);
		return false;
	}

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

static FBX_Node fbx_parse_node(void* buffer, StackAllocator* memory, int* stack_allocs_count, FILE* file) {
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

	node.name = cast(char*)stack_alloc(memory, node.name_length + 1, 1);
	fread(node.name, node.name_length, 1, file);
	node.name[node.name_length] = '\0';
	(*stack_allocs_count)++;

	
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

	// TODO: handle null records?



	int property_count = node.num_properties;
	if (property_count != 0) {

		node.properties = cast(FBX_Property*)stack_alloc(memory, property_count * sizeof(FBX_Property), 1);
		assert(node.properties != NULL);
		(*stack_allocs_count)++;

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

							node.properties[i].array.v_data = stack_alloc(memory, array_size_in_bytes, 1);
							fread(node.properties[i].array.v_data, array_size_in_bytes, 1, file);
							(*stack_allocs_count)++;
							break;
						}
						case 1: {
							uint32_t compressed_size_in_bytes = node.properties[i].array.compressed_length;
							z_stream infstream;
							infstream.zalloc = Z_NULL;
							infstream.zfree = Z_NULL;
							infstream.opaque = Z_NULL;

							// Prepare the data array with the expected size
							node.properties[i].array.v_data = stack_alloc(memory, array_size_in_bytes, 1);
							(*stack_allocs_count)++;

							// Alloc some data to be inflated into
							// once we inflated, we'll pop this off the stack
							void* inflate_buffer = stack_alloc(memory, compressed_size_in_bytes, 1);

							fread(inflate_buffer, compressed_size_in_bytes, 1, file);
							infstream.avail_in = (uInt)compressed_size_in_bytes;
							infstream.next_in = (Bytef *)inflate_buffer; // input char array
							infstream.avail_out = (uInt)array_size_in_bytes; // size of output
							infstream.next_out = (Bytef *)node.properties[i].array.v_data; // output char array


							inflateInit(&infstream);
							inflate(&infstream, Z_NO_FLUSH);
							inflateEnd(&infstream);
							// Remove the compressed data off the stack
							stack_pop(memory);

							break;

						}
					}
					break;
				}


				// Special types
				case 'R': {
					fread(buffer, 4,1, file);
					uint32_t length = *cast(uint32_t*)buffer;

					node.properties[i].special.length = length;
					node.properties[i].special.raw_binary_data = stack_alloc(memory, length + 1, 1);
					fread(cast(char*)node.properties[i].special.raw_binary_data, length, 1,file);
					(*stack_allocs_count)++;
					break;
				}

				case 'S': {
					fread(buffer, 4,1, file);
					uint32_t length = *cast(uint32_t*)buffer;
					node.properties[i].special.length = length;
					node.properties[i].special.str_data = cast(char*)stack_alloc(memory, length + 1, 1);
					fread(node.properties[i].special.str_data, length, 1, file);
					node.properties[i].special.str_data[length] = '\0';
					char* debug_str = node.properties[i].special.str_data;
					(*stack_allocs_count)++;
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
		FBX_Node temp = fbx_parse_node(buffer, memory, stack_allocs_count, file);

		FBX_Node* child_node = cast(FBX_Node*) stack_alloc(memory, sizeof(temp), 1);		
		*child_node = temp;
		(*stack_allocs_count)++;
		
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
	//if (node.num_properties == 0) {
		// Pass the null record
	rollback = ftell(file);
	fread(buffer, 13,1, file);
	if (!is_null_node(buffer)) {
		// Roll back
		fseek(file, rollback, SEEK_SET);
	}
		

	
	return node;
}



FBX_Import_Data import_fbx(const char* filename, StackAllocator* memory, bool y_is_up) {

	FBX_Import_Data import;
	import.successfully_imported = false;
	import.static_mesh_count = 0;
	import.meshes = NULL;
	

	// Since we may be doing many small string allocations
	// When we convert to our custom mesh format, we want to rewind all the stack allocs
	int stack_allocs_count = 0;

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
		return import;
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
		return import;
	}

	FBX_Node root;
	root.child_count = 0;
	root.first_child = NULL;
	root.next_sibling = NULL;

	for (;;) {
		FBX_Node temp = fbx_parse_node(buffer, memory, &stack_allocs_count, file);

		FBX_Node* child_node = cast(FBX_Node*) stack_alloc(memory, sizeof(temp), 1);
		*child_node = temp;
		stack_allocs_count++;

		if (root.child_count == 0) {
			root.first_child = child_node;
		} else {
			FBX_Node* child = root.first_child;
			while (child->next_sibling != NULL) {
				child = child->next_sibling;
			}
			child->next_sibling = child_node;
		}
		root.child_count++;

		
		if (child_node->end_offset == 0) break;
		bool s = fseek(file, child_node->end_offset, SEEK_SET);
	}

	

	if (file) {
		
		err = fclose(file);
		if (err == 0) {
			debug_print("Closed obj %s\n", filename);
		} else {
			debug_print("Cannot close obj %s\n", filename);
		}
	}

	// Parse fbx tree into our mesh

	
	FBX_Node* first_level_node;
		
	first_level_node = root.first_child;
	
	// Move to object node
	while (strcmp(first_level_node->name, "Objects") != 0) {
		first_level_node = first_level_node->next_sibling;
	}

	FBX_Node* node_attr = first_level_node->first_child;
	FBX_Node* obj_node = node_attr->next_sibling;

	int obj_count = first_level_node->child_count - 1;
	// Loop over all child nodes of the object node
	int geo_index = 0;
	while (obj_node != NULL) {
		// Hit a geometry node
		if (strcmp(obj_node->name, "Geometry") == 0) {
			StaticMesh* static_mesh = cast(StaticMesh*) malloc(sizeof(StaticMesh));
			static_mesh->vertex_count = 0;
			static_mesh->index_count = 0;
			static_mesh->normal_count = 0;
			static_mesh->uv_count = 0;
			static_mesh->uv_index_count = 0;
			static_mesh->pos = NULL;
			static_mesh->normal = NULL;
			static_mesh->texcoords = NULL;
			static_mesh->color = NULL;
			static_mesh->indices = NULL;
			
			import.static_mesh_count++;
			stb_sb_push(import.meshes, static_mesh);
			

			FBX_Node* geo_node = obj_node->first_child;
			// Loop over child nodes of the geometry node
			while (geo_node != NULL) {

				if (strcmp(geo_node->name, "Properties70") == 0) {
					// Parse properties
				}
				else if ((strcmp(geo_node->name, "Vertices") == 0)) {
					// parse vertices
					double* vertices = geo_node->properties->array.d_data;
					const int vertex_count = geo_node->properties->array.array_length / 3;
					static_mesh->pos = NULL;
					static_mesh->vertex_count = vertex_count;
					stb_sb_add(static_mesh->pos, vertex_count);
					
					for (int i = 0; i < vertex_count; i++) {
						int base_vertex_index = 3 * i;
						if (y_is_up) {
							static_mesh->pos[i] = Vec3f(vertices[base_vertex_index], vertices[base_vertex_index + 2], vertices[base_vertex_index + 1]);
						} else {
							static_mesh->pos[i] = Vec3f(vertices[base_vertex_index], vertices[base_vertex_index + 1], vertices[base_vertex_index + 2]);
						}
						
					}
					
				}
				else if ((strcmp(geo_node->name, "PolygonVertexIndex") == 0)) {
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
					static_mesh->indices = NULL;

					// TODO: performance of prefilling
					//sb_add(static_mesh->indices, estimated_index_count);
					
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
							polygon[polygon_index] = index;
							polygon_index++;
						}
						switch (polygon_indices_count) {
							
							// TODO: since we will have multiple models per fbx file, our static mesh wont cut it anymore
							// it may know contain multiple meshes
							
							case 3: {
								// Simple tri, just copy
								
								//if (current_index_count < estimated_index_count) {
//									static_mesh->indices[current_index_count] = Vec3i(polygon[0], polygon[1], polygon[2]);
								//} else {
								if (y_is_up) {
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[2], polygon[1]));
								} else {
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[1], polygon[2]));
								}
									
//								}
								current_index_count++;
								break;
							}
							case 4: {
								// Quad to tris
								
								//if (current_index_count < estimated_index_count - 2) {
//									static_mesh->indices[current_index_count] = Vec3i(polygon[0], polygon[1], polygon[2]);
									//static_mesh->indices[current_index_count + 1] = Vec3i(polygon[0], polygon[2], polygon[3]);
								//} else {
								// TODO: note that since we are not prefilling the array at the start, we'll do many reallocs if we have a large number of indices
								// as of right now this is just done in the simplest way possible, in the future if we need more performance we can optimize it here
								if (y_is_up) {
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[2], polygon[1]));
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[3], polygon[2]));
								} else {
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[1], polygon[2]));
									sb_push(static_mesh->indices, Vec3i(polygon[0], polygon[2], polygon[3]));
								}
									
								//}
								current_index_count += 2;
								
								break;
							}
								
						}
						// TODO Convert polygon to tri mesh
						//assert(polygon_indices_count <= 4);
						

					}
					static_mesh->index_count = current_index_count;
					
				}
				else if ((strcmp(geo_node->name, "Edges") == 0)) {
					// edges
				}
				else if ((strcmp(geo_node->name, "LayerElementNormal") == 0)) {
					// parse normals
					FBX_Node* normal_node_children = geo_node->first_child;
					while (normal_node_children != NULL) {

						if (strcmp(normal_node_children->name, "Normals") == 0) {
							// Parse normals
							
							double* normals = normal_node_children->properties->array.d_data;
							const int normal_count = normal_node_children->properties->array.array_length / 3;
							static_mesh->normal = NULL;
							static_mesh->normal_count = normal_count;

							stb_sb_add(static_mesh->normal, normal_count);

							for (int i = 0; i < normal_count; i++) {
								int base_normal_index = 3 * i;
								static_mesh->normal[i] = Vec3f(normals[base_normal_index], normals[base_normal_index + 1], normals[base_normal_index + 2]);
							}
						}

						normal_node_children = normal_node_children->next_sibling;
						
					}
				}
				else if ((strcmp(geo_node->name, "LayerElementMaterial") == 0)) {
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
							static_mesh->uv_count = uvs_count;
							static_mesh->texcoords = NULL;
							

							stb_sb_add(static_mesh->texcoords, uvs_count);

							for (int i = 0; i < uvs_count; i++) {
								int base_uv_index = 2 * i;
								static_mesh->texcoords[i] = Vec2f(uvs[base_uv_index], uvs[base_uv_index + 1]);
							}
							
						}

						if (strcmp(uv_node_child->name, "UVIndex") == 0) {
							// Parse normals

							int32_t* uv_indices = uv_node_child->properties->array.i_data;
							const int uv_indices_count = uv_node_child->properties->array.array_length;
							static_mesh->uv_index_count = uv_indices_count;
							//static_mesh->texcoords = NULL;
							//DEBUG_BREAK;

							//stb_sb_add(static_mesh->texcoords, uvs_count);

							//for (int i = 0; i < uvs_count; i++) {
								//int base_uv_index = 2 * i;
								//static_mesh->texcoords[i] = Vec2f(uvs[base_uv_index], uvs[base_uv_index + 1]);
							//}
							//DEBUG_BREAK;
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
		obj_node = obj_node->next_sibling;
	}
	



	

	// rewind stack allocs
	for (int i = 0; i < stack_allocs_count; i++) {
		stack_pop(memory);
	}


	



	import.successfully_imported = true;
	return import;
		
}

void free_fbx_import(FBX_Import_Data* import) {
	for (int i = 0; i < import->static_mesh_count; i++) {
		free(import->meshes[i]);
	}
	sb_free(import->meshes);
}



void init_static_mesh_manager(StaticMeshManager* manager) {
	map_init(&manager->id_map);
	manager->count = 0;
	manager->meshes = NULL;
}
void destroy_static_mesh_manager(StaticMeshManager* manager) {
	stb_sb_free(manager->meshes);
	map_destroy(&manager->id_map);
}
void entity_add_mesh_component(StaticMeshManager* manager, Entity entity) {
	map_put(&manager->id_map, entity.id, manager->count);
	manager->count++;
	stb_sb_push(manager->meshes, StaticMesh());
}

void entity_remove_mesh_component(StaticMeshManager* manager, Entity entity) {
	// See if this entity even has a mesh
	MapResult<uint64_t> result = map_get(&manager->id_map, entity.id);
	// There is no result, return early and do nothing
	if (!result.found) return;

	uint64_t index = result.value;
	// Get the last mesh in the list to swap with
	StaticMesh last = manager->meshes[manager->count - 1];
	// swap the last mesh at the current index we are removing from
	manager->meshes[index] = last;
	manager->count--;
	// Remove the entity from the index map
	map_remove(&manager->id_map, entity.id);
}

void free_static_mesh(StaticMesh* mesh) {
	stb_sb_free(mesh->pos);
	stb_sb_free(mesh->indices);
	stb_sb_free(mesh->normal);
	stb_sb_free(mesh->texcoords);
	stb_sb_free(mesh->color);
}
