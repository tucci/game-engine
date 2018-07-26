#pragma once



#include "Vec.h"
// TODO: remove need for sdl here
#include "SDL.h"

typedef struct SurfaceData{
	SDL_Surface* surface;
	unsigned char* data;
} SurfaceData;


typedef struct ObjModel {
	Vec4f* verts;
	Vec2f* texcoords;
	Vec3f* normals;

	Vec3i* v_id;
	Vec3i* vt_id;
	Vec3i* vn_id;

	int vert_count;
	int face_count;

	// Needs a reference to the sdl image
	SurfaceData diffuse;

	

} ObjModel;


float parse_float(char** stream);
float parse_int(char** stream);

void load_obj(const char* filename, ObjModel* model);

void free_obj(ObjModel* model);


void load_image(char* filename, SurfaceData* sd);