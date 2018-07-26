#pragma once


#include <stdio.h>
#include <stdlib.h>
//#include <string.h>


#include "Containers.h"

#define LINE_SIZE 256


typedef struct ObjModel {
	Vec4f* verts;
	Vec2f* texcoords;
	Vec3f* normals;

	Vec3i* v_id;
	Vec3i* vt_id;
	Vec3i* vn_id;

	int vert_count;
	int face_count;


	

} ObjModel;


float inline parse_float(char** stream) {
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

float inline parse_int(char** stream) {
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




void inline load_obj(const char* filename, ObjModel* model) {
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
		printf("The file was opened\n");
	} else {
		printf("The file was not opened\n");
	}



	char line[LINE_SIZE];

	const char * const delim_space = " ";
	const char * const delim_fslash = "/";
	

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
							v.x = parse_float(&stream);
							while (*stream == ' ') { stream++; }// skip spaces
							v.y = parse_float(&stream);
							while (*stream == ' ') { stream++; }// skip spaces
							v.z = parse_float(&stream);
							v.w = 1;
							stb_sb_push(model->verts, v);
						break;
					case 't': // Textures
						stream++;
						while (*stream == ' ') { stream++; }// skip spaces
						v.x = parse_float(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.y = parse_float(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.z = parse_float(&stream);
						stb_sb_push(model->texcoords, v.xyz_.xy_);
						break;
					case 'n': // Normals
						stream++;
						while (*stream == ' ') { stream++; }// skip spaces
						v.x = parse_float(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.y = parse_float(&stream);
						while (*stream == ' ') { stream++; }// skip spaces
						v.z = parse_float(&stream);
						stb_sb_push(model->normals, v.xyz_);
						break;
					default:
						break;
				}

				break;
			case 'f':
				stream++;
				for (int i = 0; i < 3; i++) {
					while (*stream == ' ') { stream++; }// skip spaces
					v_id.data[i] = parse_int(&stream);
					if (*stream == '/') {
						has_vts = true;
						stream++;
						vt_id.data[i] = parse_int(&stream);
					}
					if (*stream == '/') {
						has_vns = true;
						stream++;
						vn_id.data[i] = parse_int(&stream);
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

	printf("Vertex count %d\n", stb_sb_count(model->verts));
	printf("Face count %d\n", stb_sb_count(model->v_id));

	if (file) {
		err = fclose(file);
		if (err == 0) {
			printf("The file was closed\n");
		} else {
			printf("The file was not closed\n");
		}
	}

	model->vert_count = stb_sb_count(model->verts);
	model->face_count = stb_sb_count(model->v_id);
	

	
}


void inline free_obj(ObjModel* model) {
	stb_sb_free(model->verts);
	stb_sb_free(model->texcoords);
	stb_sb_free(model->normals);

	stb_sb_free(model->v_id);
	stb_sb_free(model->vt_id);
	stb_sb_free(model->vn_id);
}
