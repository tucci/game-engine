#pragma once


#include "Core/ECS/Component/StaticMesh.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>


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





void obj_to_static_mesh(const char* filename, StaticMesh* static_mesh, LinearAllocator* memory) {

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
		debug_print("The file was opened\n");
	} else {
		debug_print("The file was not opened\n");
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

	debug_print("Vertex count %d\n", stb_sb_count(model.verts));
	debug_print("Face count %d\n", stb_sb_count(model.v_id));

	if (file) {
		err = fclose(file);
		if (err == 0) {
			debug_print("The file was closed\n");
		} else {
			debug_print("The file was not closed\n");
		}
	}

	model.vert_count = stb_sb_count(model.verts);
	model.texcoord_count = stb_sb_count(model.texcoords);
	model.normal_count = stb_sb_count(model.normals);
	model.face_count = stb_sb_count(model.v_id);
	model.tex_indice_count = stb_sb_count(model.vt_id);



	// Copy and Convert our model into our static mesh
	int index_count = model.face_count;
	static_mesh->index_count = index_count;
	static_mesh->indices = cast(Vec3i*)linear_alloc(memory, index_count * sizeof(Vec3i), 4);


	static_mesh->vertex_count = index_count * 3;
	static_mesh->pos = cast(Vec3f*)linear_alloc(memory, static_mesh->vertex_count * sizeof(Vec3f), 4);
	static_mesh->texcoords = cast(Vec2f*)linear_alloc(memory, static_mesh->vertex_count * sizeof(Vec2f), 4);
	static_mesh->normal = cast(Vec3f*)linear_alloc(memory, static_mesh->vertex_count * sizeof(Vec3f), 4);





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

}
