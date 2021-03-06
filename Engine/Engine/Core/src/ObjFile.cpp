#pragma once

#include "ObjFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>


#include "Common/stretchy_buffer.h"

#include "debug_macros.h"


// NOTE: these are deprecated, use obj_to_static_mesh in static mesh to load into our arenas
// This is used just for the legacy software renderer

static float parse_float2(char** stream) {
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

static int parse_int2(char** stream) {
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

// NOTE: deprecated. only used in the software renderer
void load_obj(const char* filename, ObjModel* model) {
	model->verts = 0;
	model->texcoords = 0;
	model->normals = 0;

	model->v_id = 0;
	model->vt_id = 0;
	model->vn_id = 0;

	





	FILE* file;
	errno_t err;

	err = fopen_s(&file, filename, "r");

	if (err == 0) {
		debug_print("The file was opened\n");
	} else {
		debug_print("The file was not opened\n");
		return;
	}



	char line[256];

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
						while (*stream == ' ') { stream++; }// skip spaces
						v.x = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.y = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.z = parse_float2(&stream);
						v.w = 1;
						stb_sb_push(model->verts, v);
						break;
					case 't': // Textures
						stream++;
						while (*stream == ' ') { stream++; }// skip spaces
						v.x = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.y = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.z = parse_float2(&stream);
						stb_sb_push(model->texcoords, v.xyz.xy);
						break;
					case 'n': // Normals
						stream++;
						while (*stream == ' ') { stream++; }// skip spaces
						v.x = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.y = parse_float2(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.z = parse_float2(&stream);
						stb_sb_push(model->normals, v.xyz);
						break;
					default:
						break;
				}

				break;
			case 'f':
				stream++;
				for (int i = 0; i < 3; i++) {
					while (*stream == ' ') { stream++; }// skip spaces
					v_id.data[i] = parse_int2(&stream) - 1;
					if (*stream == '/') {
						has_vts = true;
						stream++;
						vt_id.data[i] = parse_int2(&stream) - 1;
					}
					if (*stream == '/') {
						has_vns = true;
						stream++;
						vn_id.data[i] = parse_int2(&stream) - 1;
					}
				}

				stb_sb_push(model->v_id, v_id);
				if (has_vts) {
					stb_sb_push(model->vt_id, vt_id);
				}
				if (has_vns) {
					stb_sb_push(model->vn_id, vn_id);
				}


				break;
			default:
				break;
		}

	}

	debug_print("Vertex count %d\n", stb_sb_count(model->verts));
	debug_print("Face count %d\n", stb_sb_count(model->v_id));

	if (file) {
		err = fclose(file);
		if (err == 0) {
			debug_print("The file was closed\n");
		} else {
			debug_print("The file was not closed\n");
		}
	}

	model->vert_count = stb_sb_count(model->verts);
	model->texcoord_count = stb_sb_count(model->texcoords);
	model->normal_count = stb_sb_count(model->normals);
	model->face_count = stb_sb_count(model->v_id);
	model->tex_indice_count = stb_sb_count(model->vt_id);




}

void free_obj(ObjModel* model) {
	stb_sb_free(model->verts);
	stb_sb_free(model->texcoords);
	stb_sb_free(model->normals);

	stb_sb_free(model->v_id);
	stb_sb_free(model->vt_id);
	stb_sb_free(model->vn_id);
	
}
