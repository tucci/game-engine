#pragma once

#include "Core/ECS/EntityManager.h"	

StaticMesh* get_static_mesh(EntityManager* manager, Entity entity);
bool set_static_mesh(EntityManager* manager, Entity entity, StaticMesh* mesh);