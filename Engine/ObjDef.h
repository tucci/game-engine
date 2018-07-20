#pragma once


#include <stdio.h>
#include <stdlib.h>

#include "Containers.h"


typedef struct ObjModel {
	Vec4f* verts;
	Vec3i* faces;
	/*Vec3f* vt_arr;
	Vec3f* vn_arr;
	Vec3f* vp_arr;*/

} ObjModel;


void inline load_obj(const char* filename, ObjModel* model) {
	model->verts = 0;
	model->faces = 0;



	FILE* file;
	errno_t err;

	err = fopen_s(&file, filename, "r"); 

	if (err == 0) {
		printf("The file was opened\n");
	} else {
		printf("The file was not opened\n");
	}



	char line[256];

	while (fgets(line, sizeof(line), file)) {

		

		char* token = strtok(line, " ");



		// TODO: see how strtok works under the hood. see if it does mem allocs
		// TODO: see if using " ", and "/" causes small str allocs
		if (strcmp("v", token) == 0) {
			Vec4f v;
			token = strtok(NULL, " ");
			v.x = atof(token);
			token = strtok(NULL, " ");
			v.y = atof(token);
			token = strtok(NULL, " ");
			v.z = atof(token);
			v.w = 1;
			stb_sb_push(model->verts, v);
		}
		else if (strcmp("f", token) == 0) {
			// TODO: handle faces without /
			Vec3i f;
			token = strtok(NULL, " ");
			//token = strtok(NULL, "/");
			f.x = atoi(token);
			token = strtok(NULL, " ");
			//token = strtok(NULL, "/");
			f.y = atoi(token);
			token = strtok(NULL, " ");
			//token = strtok(NULL, "/");
			f.z = atoi(token);
			stb_sb_push(model->faces, f);

		}
		
	}

	printf("Vertex count %d\n", stb_sb_count(model->verts));
	printf("Face count %d\n", stb_sb_count(model->faces));

	if (file) {
		err = fclose(file);
		if (err == 0) {
			printf("The file was closed\n");
		} else {
			printf("The file was not closed\n");
		}
	}

	
}


void inline free_obj(ObjModel* model) {
	stb_sb_free(model->verts);
	stb_sb_free(model->faces);
}
