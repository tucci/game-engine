#pragma once

#include "Core/ECS/Component/StaticMesh.h"

typedef struct RenderComponent {
	StaticMesh* mesh_ref;
} RenderComponent;