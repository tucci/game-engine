#pragma once


#include "Core/ECS/Component/StaticMesh.h"
#include "Common/common_macros.h"


void make_plane(StaticMesh* mesh, StackAllocator* memory);
void make_cube(StaticMesh* mesh, StackAllocator* memory);

void make_uv_sphere(StaticMesh* mesh, int lats, int longs, StackAllocator* memory);