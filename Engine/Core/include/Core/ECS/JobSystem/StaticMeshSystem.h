#pragma once

#include "Core/ECS/EntityManager.h"	

StaticMeshID get_static_mesh(EntityManager* manager, Entity entity);
bool set_static_mesh(EntityManager* manager, Entity entity, StaticMeshID id);