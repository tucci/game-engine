#pragma once


#include "../StaticMesh.h"
#include "../../Common/common_macros.h"


void make_plane(StaticMesh* mesh, LinearAllocator* memory);
void make_cube(StaticMesh* mesh, LinearAllocator* memory);

void make_uv_sphere(StaticMesh* mesh, int lats, int longs, LinearAllocator* memory);